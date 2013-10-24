
// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerVolumeDisplayControllerModuleWidget.h"
#include "ui_qSlicerVolumeDisplayControllerModuleWidget.h"


// CTK includes
#include <ctkVTKColorTransferFunction.h>
#include <ctkTransferFunctionGradientItem.h>
#include <ctkTransferFunctionScene.h>
#include <ctkTransferFunctionBarsItem.h>
#include <ctkVTKHistogram.h>

// MRML includes
#include "vtkMRMLColorNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"

#include "qSlicerVolumeDisplayWidget.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkSlicerVolumeRenderingLogic.h"

// STD includes
#include <limits>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeDisplayControllerModuleWidgetPrivate: public Ui_qSlicerVolumeDisplayControllerModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerVolumeDisplayControllerModuleWidget);
protected:
	qSlicerVolumeDisplayControllerModuleWidget* const q_ptr;

public:
  qSlicerVolumeDisplayControllerModuleWidgetPrivate(qSlicerVolumeDisplayControllerModuleWidget& object);
  vtkMRMLVolumeRenderingDisplayNode* createVolumeRenderingDisplayNode(vtkMRMLVolumeNode* volumeNode);

  vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidgetPrivate
::qSlicerVolumeDisplayControllerModuleWidgetPrivate(
	qSlicerVolumeDisplayControllerModuleWidget& object)
	:q_ptr(&object)
{
	VolumeRenderingDisplayNode = 0;
}


// --------------------------------------------------------------------------
vtkMRMLVolumeRenderingDisplayNode* qSlicerVolumeDisplayControllerModuleWidgetPrivate::createVolumeRenderingDisplayNode(vtkMRMLVolumeNode* volumeNode)
{
	Q_Q(qSlicerVolumeDisplayControllerModuleWidget);

	//VolumeRendering 
	vtkSlicerVolumeRenderingLogic *volumeRenderingLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(
		qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	if(!volumeRenderingLogic)
		return 0;

	vtkMRMLVolumeRenderingDisplayNode* displayNode = volumeRenderingLogic->CreateVolumeRenderingDisplayNode();
	q->mrmlScene()->AddNode(displayNode);
	displayNode->Delete();

	vtkMRMLVolumePropertyNode *propNode = NULL;
	vtkMRMLAnnotationROINode  *roiNode = NULL;

	int wasModifying = displayNode->StartModify();
	// Init the volume rendering without the threshold info
	// of the Volumes module...
	displayNode->SetIgnoreVolumeDisplayNodeThreshold(1);
	volumeRenderingLogic->UpdateDisplayNodeFromVolumeNode(displayNode, volumeNode,
		&propNode, &roiNode);
	// ... but then apply the user settings.
	//displayNode->SetIgnoreVolumeDisplayNodeThreshold(
	//	this->IgnoreVolumesThresholdCheckBox->isChecked());
	bool wasLastVolumeVisible = this->Visibility3DCheckBox->isChecked();
	displayNode->SetVisibility(wasLastVolumeVisible);
	
	//foreach (vtkMRMLViewNode* viewNode,
	//	this->ViewCheckableNodeComboBox->checkedViewNodes())
	//{
	//	displayNode->AddViewNodeID(viewNode ? viewNode->GetID() : 0);
	//}
	displayNode->EndModify(wasModifying);
	if (volumeNode)
	{
		volumeNode->AddAndObserveDisplayNodeID(displayNode->GetID());
	}
	return displayNode;
}


//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidget::qSlicerVolumeDisplayControllerModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerVolumeDisplayControllerModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidget::~qSlicerVolumeDisplayControllerModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerVolumeDisplayControllerModuleWidget::setup()
{
  Q_D(qSlicerVolumeDisplayControllerModuleWidget);
  d->setupUi(this);

  QObject::connect(d->Visibility3DCheckBox,
	  SIGNAL(toggled(bool)),
	  this, SLOT(onVisibilityChanged(bool)));

  this->Superclass::setup();

  connect(this,SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
	  d->ActiveVolumeNodeSelector,SLOT(setMRMLScene(vtkMRMLScene*)));

  QObject::connect(d->ActiveVolumeNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(setMRMLVolumeNode(vtkMRMLNode*)));
   
}


void qSlicerVolumeDisplayControllerModuleWidget::setMRMLVolumeNode( vtkMRMLNode* node )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	 vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
	if(!volumeNode)
	{
		d->VolumeRenderingDisplayNode = 0;
		return;
	}

	//2D Display 
	d->VolumeDisplayWidget->setMRMLVolumeNode(node);


	//VolumeRendering 
	 vtkSlicerVolumeRenderingLogic *volumeRenderingLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(
		 qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	 if(!volumeRenderingLogic)
		 return;


	 // see if the volume has any display node for a current viewer
	 vtkMRMLVolumeRenderingDisplayNode *dnode =
		 volumeRenderingLogic->GetFirstVolumeRenderingDisplayNode(volumeNode);

	 if (!this->mrmlScene()->IsClosing())
	 {
		 if (!dnode)
		 {
			 dnode = d->createVolumeRenderingDisplayNode(volumeNode);
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

	 d->VolumeRenderingDisplayNode = dnode;
}

// --------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* qSlicerVolumeDisplayControllerModuleWidget
::mrmlVolumeNode()const
{
	Q_D(const qSlicerVolumeDisplayControllerModuleWidget);
	return vtkMRMLScalarVolumeNode::SafeDownCast(
		d->ActiveVolumeNodeSelector->currentNode());
}


// --------------------------------------------------------------------------
vtkMRMLVolumeRenderingDisplayNode* qSlicerVolumeDisplayControllerModuleWidget
::mrmlVolumeRenderingDisplayNode()const
{
	Q_D(const qSlicerVolumeDisplayControllerModuleWidget);
	//return vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(
	//	d->DisplayNodeComboBox->currentNode());
	return NULL;
}



// --------------------------------------------------------------------------
void qSlicerVolumeDisplayControllerModuleWidget::onVisibilityChanged(bool visible)
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	if (!d->VolumeRenderingDisplayNode)
	{
		return;
	}
	d->VolumeRenderingDisplayNode->SetVisibility(visible);
}