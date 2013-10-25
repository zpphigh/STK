#include "stkSlicerVolumeNodeDisplayHelper.h"

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerAbstractCoreModule.h>
#include <qSlicerAbstractModule.h>

#include <vtkRenderer.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>
#include <vtkMRMLVolumePropertyNode.h>


#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>

#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSliceLayerLogic.h>
#include <vtkSlicerVolumesLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkSlicerVolumeRenderingLogic.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSliceCompositeNode.h>

#include <vtkMRMLVolumeRenderingDisplayNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLLayoutNode.h>
#include <vtkSlicerVolumesLogic.h>

#include <qMRMLSliceWidget.h>
#include <qMRMLSliceControllerWidget.h>


#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkImageResample.h>
#include <vtkNew.h>
#include "vtkMarchingCubes.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkDecimatePro.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkReverseSense.h"
#include "vtkPolyDataNormals.h"
#include "vtkStripper.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkImageChangeInformation.h"
#include "vtkDebugLeaks.h"
#include "vtkTriangleFilter.h"
#include "vtkMassProperties.h"

class stkSlicerVolumeNodeDisplayHelperPrivate
{
public:

};

stkSlicerVolumeNodeDisplayHelper::stkSlicerVolumeNodeDisplayHelper(QObject *parent)
: QObject(parent),d_ptr(new stkSlicerVolumeNodeDisplayHelperPrivate)
{
	Q_D(stkSlicerVolumeNodeDisplayHelper);

}

stkSlicerVolumeNodeDisplayHelper::~stkSlicerVolumeNodeDisplayHelper()
{
	Q_D(stkSlicerVolumeNodeDisplayHelper);
}


// --------------------------------------------------------------------------
vtkMRMLVolumeRenderingDisplayNode* stkSlicerVolumeNodeDisplayHelper::CreateVolumeRenderingDisplayNode(vtkMRMLVolumeNode* volumeNode)
{
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	if(!scene)
		return 0;

	//VolumeRendering Logic
	vtkSlicerVolumeRenderingLogic *vrLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(
		qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	if(!vrLogic)
		return 0;

	vrLogic->SetDefaultRenderingMethod("vtkMRMLGPUTextureMappingVolumeRenderingDisplayNode");

	vtkMRMLVolumeRenderingDisplayNode* displayNode = vrLogic->CreateVolumeRenderingDisplayNode();
	scene->AddNode(displayNode);
	displayNode->Delete();

	vtkMRMLVolumePropertyNode *propNode = NULL;
	vtkMRMLAnnotationROINode  *roiNode = NULL;

	int wasModifying = displayNode->StartModify();
	// Init the volume rendering without the threshold info
	// of the Volumes module...
	displayNode->SetIgnoreVolumeDisplayNodeThreshold(1);
	vrLogic->UpdateDisplayNodeFromVolumeNode(displayNode, volumeNode, &propNode, &roiNode);
	// ... but then apply the user settings.
	displayNode->SetIgnoreVolumeDisplayNodeThreshold(0);
	displayNode->SetVisibility(1);

	// find the fist view node in the scene
	//vtkMRMLViewNode* viewNode = vtkMRMLViewNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLViewNode"));
	//displayNode->AddViewNodeID(viewNode ? viewNode->GetID() : 0);

	displayNode->EndModify(wasModifying);
	if (volumeNode)
	{
		volumeNode->AddAndObserveDisplayNodeID(displayNode->GetID());
	}
	return displayNode;
}



vtkMRMLVolumeRenderingDisplayNode* createVolumeRenderingDisplayNode(vtkMRMLVolumeNode* volumeNode, vtkColorTransferFunction* colorTransfer, vtkPiecewiseFunction* piecewiseFunction)
{
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	vtkSlicerVolumeRenderingLogic* volumeRenderLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(qSlicerApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	if (!volumeRenderLogic) return NULL;

	volumeRenderLogic->SetDefaultRenderingMethod("vtkMRMLGPUTextureMappingVolumeRenderingDisplayNode");
	vtkMRMLVolumeRenderingDisplayNode *displayNode = volumeRenderLogic->CreateVolumeRenderingDisplayNode();//强制使用GPU体绘制
	scene->AddNode(displayNode);
	displayNode->Delete();

	vtkMRMLVolumePropertyNode *propNode = NULL;
	vtkMRMLAnnotationROINode  *roiNode = NULL;

	int wasModifying = displayNode->StartModify();
	// Init the volume rendering without the threshold info
	// of the Volumes module...
	displayNode->SetIgnoreVolumeDisplayNodeThreshold(1);
	volumeRenderLogic->UpdateDisplayNodeFromVolumeNode(displayNode, volumeNode, &propNode, &roiNode);
	// ... but then apply the user settings.
	displayNode->SetIgnoreVolumeDisplayNodeThreshold(0);
	displayNode->SetVisibility(0);

	//setDefaultPreset
	vtkMRMLVolumePropertyNode *propertyNode = displayNode->GetVolumePropertyNode();
	if (propertyNode)
	{
		propertyNode->HideFromEditorsOff();
		propertyNode->SelectableOn();
		propertyNode->SelectedOff();

		propertyNode->GetVolumeProperty()->ShadeOn();
		propertyNode->GetVolumeProperty()->SetAmbient(0.1);
		propertyNode->GetVolumeProperty()->SetDiffuse(0.9);
		propertyNode->GetVolumeProperty()->SetSpecular(0.2);
		propertyNode->GetVolumeProperty()->SetSpecularPower(10.0);
		propertyNode->GetVolumeProperty()->SetScalarOpacityUnitDistance(0.8919);

		if (piecewiseFunction) propertyNode->SetScalarOpacity(piecewiseFunction);
		if (colorTransfer) propertyNode->SetColor(colorTransfer);
	}

	// find the fist view node in the scene
	vtkMRMLViewNode* viewNode = vtkMRMLViewNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLViewNode"));

	displayNode->AddViewNodeID(viewNode ? viewNode->GetID() : 0);
	displayNode->EndModify(wasModifying);
	if (volumeNode)
	{
		volumeNode->AddAndObserveDisplayNodeID(displayNode->GetID());
	}

	return displayNode;
}





// --------------------------------------------------------------------------
void stkSlicerVolumeNodeDisplayHelper::Display3DVolume(std::string nodeID, vtkColorTransferFunction* colorTransfer, vtkPiecewiseFunction* piecewiseFunction)
{
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	vtkMRMLScalarVolumeNode* scalarNode = vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(nodeID));
	if (!scalarNode) return;

	//体绘制默认显示出来//	
	vtkSlicerVolumeRenderingLogic* volumeRenderLogic = 
		vtkSlicerVolumeRenderingLogic::SafeDownCast(qSlicerApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	if (!volumeRenderLogic) return;	

	vtkMRMLVolumeRenderingDisplayNode *dnode =
		volumeRenderLogic->GetFirstVolumeRenderingDisplayNode(scalarNode);
	if (!scene->IsClosing())
	{
		if (!dnode)
		{
			dnode = createVolumeRenderingDisplayNode(scalarNode, colorTransfer, piecewiseFunction);
		}
		else
		{
			// Because the displayable manager can only display 1 volume at
			// a time, here the displayable manager is told that the display node
			// is the new "current" display node and it should be displayed 
			// instead of whichever current one.
			dnode->Modified();
		}
	}
}


void stkSlicerVolumeNodeDisplayHelper::Display2DSlice(std::string nodeID)
{
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	vtkSlicerVolumesLogic* volumeLogic = vtkSlicerVolumesLogic::SafeDownCast(qSlicerApplication::application()->moduleManager()->module("Volumes")->logic());
	if (!volumeLogic) return;

	vtkMRMLScalarVolumeNode* scalarNode = vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(nodeID));
	if (!scalarNode) return;
	scalarNode->GetDisplayNode()->SetAndObserveColorNodeID(volumeLogic->GetColorLogic()->GetDefaultVolumeColorNodeID());
	volumeLogic->SetActiveVolumeNode(scalarNode);

	
	vtkSlicerApplicationLogic* appLogic = volumeLogic->GetApplicationLogic();
	vtkMRMLSelectionNode* selectionNode = appLogic ? appLogic->GetSelectionNode() : 0;		
	if (selectionNode)
	{
		if (vtkMRMLScalarVolumeNode::SafeDownCast(scalarNode) &&
			vtkMRMLScalarVolumeNode::SafeDownCast(scalarNode)->GetLabelMap())
		{
			selectionNode->SetReferenceActiveLabelVolumeID(scalarNode->GetID());
		}
		else
		{
			selectionNode->SetReferenceActiveVolumeID(scalarNode->GetID());
		}
		if (appLogic)
		{
			appLogic->PropagateVolumeSelection();
			// TODO: slices should probably be fitting automatically..
			appLogic->FitSliceToAll();
		}
	}


	double sliceBounds[6];
	double sliceCenter[3];
	bool boundInited = false;

	scene->InitTraversal();
	for (vtkMRMLNode *sn = NULL; (sn=scene->GetNextNodeByClass("vtkMRMLSliceNode"));) 
	{
		vtkMRMLSliceNode *snode = vtkMRMLSliceNode::SafeDownCast(sn);
		if (snode)
		{
			if (!boundInited)
			{
				vtkSmartPointer<vtkMRMLSliceLogic> sliceLogic = vtkSmartPointer<vtkMRMLSliceLogic>::New();
				sliceLogic->SetSliceNode(snode);
				sliceLogic->GetVolumeSliceBounds(scalarNode,sliceBounds);
				sliceCenter[0] = (sliceBounds[0]+sliceBounds[1])/2;
				sliceCenter[1] = (sliceBounds[2]+sliceBounds[3])/2;
				sliceCenter[2] = (sliceBounds[4]+sliceBounds[5])/2;
				boundInited =true;
			}

			snode->JumpSlice(sliceCenter[0],sliceCenter[1],sliceCenter[2]);
			snode->SetSliceVisible(1);
		}
	}
}

void stkSlicerVolumeNodeDisplayHelper::DisplayForeground2DSlice(std::string nodeID)
{
	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();
	vtkMRMLScalarVolumeNode* scalarNode = vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(nodeID));
	if (!scalarNode) return;

	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	QStringList sliceNames = layoutManager->sliceViewNames();
	foreach(QString slicename, sliceNames)
	{
		qMRMLSliceControllerWidget* SliceController = layoutManager->sliceWidget(slicename)->sliceController();
		vtkMRMLSliceLogic* SliceLogic = SliceController->sliceLogic();
		SliceLogic->StartSliceCompositeNodeInteraction(vtkMRMLSliceCompositeNode::ForegroundVolumeFlag);
		SliceController->mrmlSliceCompositeNode()->SetForegroundVolumeID(nodeID.c_str());
		SliceLogic->EndSliceCompositeNodeInteraction();

		SliceController->setForegroundHidden(false);
		SliceController->setForegroundOpacity(1);
		//SliceController->fitSliceToBackground();
	}

	vtkSlicerVolumesLogic* volumeLogic = 
		vtkSlicerVolumesLogic::SafeDownCast(qSlicerApplication::application()->moduleManager()->module("Volumes")->logic());
	if (!volumeLogic) return;

	volumeLogic->SetActiveVolumeNode(scalarNode);
	vtkMRMLScalarVolumeDisplayNode* displayNode = scalarNode->GetScalarVolumeDisplayNode();
	if (displayNode)
	{
		int disabledModify = displayNode->StartModify();
		displayNode->SetApplyThreshold(1);
		displayNode->SetAutoThreshold(0);
		displayNode->EndModify(disabledModify);
	}
}


//---------------------------------------------------------------------------
double stkSlicerVolumeNodeDisplayHelper::VolumeToGrayscaleModel(std::string InputVolume, double Threshold, std::string ModelName,double Smooth, double Decimate,bool SplitNormals,bool PointNormals,  double r, double g, double b)
{
	if (InputVolume == "") return 0;

	vtkMRMLScene* scene = qSlicerApplication::application()->mrmlScene();

	// vtk and helper variables
	vtkImageData *                    image;
	vtkWindowedSincPolyDataFilter *   smootherSinc = NULL;
	vtkDecimatePro *                  decimator = NULL;
	vtkMarchingCubes *                mcubes = NULL;
	vtkTransform *                    transformIJKtoRAS = NULL;
	vtkReverseSense *                 reverser = NULL;
	vtkTransformPolyDataFilter *      transformer = NULL;
	vtkPolyDataNormals *              normals = NULL;
	vtkStripper *                     stripper = NULL;

	vtkMRMLScalarVolumeNode* volumeNode = 
		vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(InputVolume));
	if (!volumeNode) return 0;

	vtkImageChangeInformation *ici = vtkImageChangeInformation::New();
	ici->SetInput(volumeNode->GetImageData());
	ici->SetOutputSpacing(volumeNode->GetImageData()->GetSpacing());
	ici->SetOutputOrigin(volumeNode->GetImageData()->GetOrigin());
	ici->Update();

	image = ici->GetOutput();
	image->Update();

	int extents[6];
	image->GetExtent(extents);
	if( extents[0] == extents[1] ||
		extents[2] == extents[3] ||
		extents[4] == extents[5] )
	{
		std::cerr << "The volume is not 3D.\n";
		std::cerr << "\tImage data extents: " << extents[0] << " " << extents[1] << " " << extents[2] << " "
			<< extents[3] << " " << extents[4] << " " << extents[5] << endl;
		return 0;
	}
	// Get the RAS to IJK matrix and invert it to get the IJK to RAS which will need
	// to be applied to the model as it will be built in pixel space

	vtkSmartPointer<vtkMatrix4x4> RAStoIJK = vtkSmartPointer<vtkMatrix4x4>::New();
	volumeNode->GetRASToIJKMatrix(RAStoIJK);

	transformIJKtoRAS = vtkTransform::New();
	transformIJKtoRAS->SetMatrix(RAStoIJK);
	transformIJKtoRAS->Inverse();

	mcubes = vtkMarchingCubes::New();
	mcubes->SetInput(ici->GetOutput() );
	mcubes->SetValue(0, Threshold);
	mcubes->ComputeScalarsOff();
	mcubes->ComputeGradientsOff();
	mcubes->ComputeNormalsOff();
	(mcubes->GetOutput() )->ReleaseDataFlagOn();
	mcubes->Update();

	decimator = vtkDecimatePro::New();
	decimator->SetInput(mcubes->GetOutput() );
	decimator->SetFeatureAngle(60);
	decimator->SplittingOff();
	decimator->PreserveTopologyOn();

	decimator->SetMaximumError(1);
	decimator->SetTargetReduction(Decimate);
	(decimator->GetOutput() )->ReleaseDataFlagOff();
	decimator->Update();

	if( (transformIJKtoRAS->GetMatrix() )->Determinant() < 0 )
	{
		reverser = vtkReverseSense::New();
		reverser->SetInput(decimator->GetOutput() );
		reverser->ReverseNormalsOn();
		(reverser->GetOutput() )->ReleaseDataFlagOn();
	}

	smootherSinc = vtkWindowedSincPolyDataFilter::New();
	smootherSinc->SetPassBand(0.1);
	if( Smooth == 1 )
	{
		std::cerr << "Warning: Smoothing iterations of 1 not allowed for Sinc filter, using 2" << endl;
		Smooth = 2;
	}
	if( (transformIJKtoRAS->GetMatrix() )->Determinant() < 0 )
	{
		smootherSinc->SetInput(reverser->GetOutput() );
	}
	else
	{
		smootherSinc->SetInput(decimator->GetOutput() );
	}
	smootherSinc->SetNumberOfIterations(Smooth);
	smootherSinc->FeatureEdgeSmoothingOff();
	smootherSinc->BoundarySmoothingOff();
	(smootherSinc->GetOutput() )->ReleaseDataFlagOn();
	smootherSinc->Update();

	transformer = vtkTransformPolyDataFilter::New();
	transformer->SetInput(smootherSinc->GetOutput() );
	if( (transformIJKtoRAS->GetMatrix() )->Determinant() < 0 )
	{
		transformer->SetInput(reverser->GetOutput() );
	}
	else
	{
		transformer->SetInput(decimator->GetOutput() );
	}

	transformer->SetTransform(transformIJKtoRAS);
	(transformer->GetOutput() )->ReleaseDataFlagOn();

	//计算体积
	vtkTriangleFilter *triangleFilter=vtkTriangleFilter::New();
	triangleFilter-> SetInputConnection(transformer-> GetOutputPort());
	vtkMassProperties *Mass=vtkMassProperties::New();
	Mass-> SetInputConnection (triangleFilter->GetOutputPort());
	double modelVolum = Mass->GetVolume()/1000.0;

	normals = vtkPolyDataNormals::New();
	if( PointNormals )
	{
		normals->ComputePointNormalsOn();
	}
	else
	{
		normals->ComputePointNormalsOff();
	}
	normals->SetInput(transformer->GetOutput() );
	normals->SetFeatureAngle(60);
	normals->SetSplitting(SplitNormals);
	(normals->GetOutput() )->ReleaseDataFlagOn();

	stripper = vtkStripper::New();
	stripper->SetInput(normals->GetOutput() );
	std::cout << "Stripping...\n";
	(stripper->GetOutput() )->ReleaseDataFlagOff();
	(stripper->GetOutput() )->Update();

	vtkMRMLModelNode* outputModel = vtkMRMLModelNode::New();
	outputModel->SetName(ModelName.c_str());
	outputModel->SetAndObservePolyData(stripper->GetOutput());
	scene->AddNode(outputModel);
	outputModel->Delete();

	vtkMRMLModelDisplayNode* displayNode = vtkMRMLModelDisplayNode::New();
	displayNode->SetInputPolyData(outputModel->GetPolyData());
	displayNode->SetColor(r,g,b);
	scene->SaveStateForUndo();
	scene->AddNode(displayNode);
	displayNode->SetScene(scene);

	outputModel->SetAndObserveDisplayNodeID(displayNode->GetID());
	outputModel->SetHideFromEditors(0);
	displayNode->Delete();

	// Cleanup
	if( ici )
	{
		ici->Delete();
	}
	if( transformIJKtoRAS )
	{
		transformIJKtoRAS->Delete();
	}
	if( mcubes )
	{
		mcubes->Delete();
	}
	if( decimator )
	{
		decimator->Delete();
	}
	if( reverser )
	{
		reverser->Delete();
	}
	if( smootherSinc )
	{
		smootherSinc->Delete();
	}
	if( transformer )
	{
		transformer->Delete();
	}
	if ( triangleFilter )
	{
		triangleFilter->Delete();
	}
	if ( Mass )
	{
		Mass->Delete();
	}
	if( normals )
	{
		normals->Delete();
	}
	if( stripper )
	{
		stripper->Delete();
	}

	return modelVolum;
}