
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
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLColorNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include "vtkMRMLSliceLogic.h"
#include "qMRMLSliceWidget.h"
#include "qSlicerLayoutManager.h"
#include "qMRMLSliceControllerWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"

#include "qSlicerVolumeDisplayWidget.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkSlicerVolumeRenderingLogic.h"
#include "stkSlicerVolumeNodeDisplayHelper.h"

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

  vtkMRMLSliceNode* SliceNode[3];
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
	SliceNode[0] = 0;
	SliceNode[1] = 0;
	SliceNode[2] = 0;
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

  //d->ActiveVolumeNodeSelector->setAddEnabled(false);
  //d->ActiveVolumeNodeSelector->setRenameEnabled(false);
  //d->ActiveVolumeNodeSelector->setRemoveEnabled(false);
  this->Superclass::setup();


  connect(this,SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),  d->ActiveVolumeNodeSelector,SLOT(setMRMLScene(vtkMRMLScene*)));

  QObject::connect(d->ActiveVolumeNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(setMRMLVolumeNode(vtkMRMLNode*)));


  QObject::connect(d->Visibility3DCheckBox,  SIGNAL(toggled(bool)), this, SLOT(setVR3DVisibile(bool)));
  connect(d->AxialSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceAxialVisible(bool)));
  connect(d->SagittalSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceSagittalVisible(bool)));
  connect(d->CoronalSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceCoronalVisible(bool)));


  connect(d->AxialSliceSlider, SIGNAL(valueChanged(double)),this,SLOT(setAxialSliceOffsetValue(double)));
  connect(d->SagittalSliceSlider,SIGNAL(valueChanged(double)),this,SLOT(setSagittalSliceOffsetValue(double)));
  connect(d->CoronalSliceSlider,SIGNAL(valueChanged(double)),this,SLOT(setCoronalSliceOffsetValue(double)));
   
}


void qSlicerVolumeDisplayControllerModuleWidget::setMRMLVolumeNode( vtkMRMLNode* node )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;
	
	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)	return;

	 vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
	if(!volumeNode)
	{
		d->VolumeRenderingDisplayNode = 0;
		return;
	}

	//Slice

	d->SliceNode[0] = vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeRed"));
	d->SliceNode[1] = vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeYellow"));
	d->SliceNode[2] = vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeGreen"));
	if (d->SliceNode[0])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[0]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);
		d->AxialSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->AxialSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
	}
	if (d->SliceNode[1])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[1]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);
		d->SagittalSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->SagittalSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
	}
	if (d->SliceNode[2])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[2]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);
		d->CoronalSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->CoronalSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
	}



	//Volume Display 
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
			 dnode = stkSlicerVolumeNodeDisplayHelper::CreateVolumeRenderingDisplayNode(volumeNode);
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






void qSlicerVolumeDisplayControllerModuleWidget::setAxialSliceOffsetValue( double newValue)
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	d->SliceNode[0]->SetSliceOffset(newValue);	
}

void qSlicerVolumeDisplayControllerModuleWidget::setSagittalSliceOffsetValue( double newValue)
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	d->SliceNode[1]->SetSliceOffset(newValue);	
}

void qSlicerVolumeDisplayControllerModuleWidget::setCoronalSliceOffsetValue( double newValue)
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	d->SliceNode[2]->SetSliceOffset(newValue);	
}


// --------------------------------------------------------------------------
void qSlicerVolumeDisplayControllerModuleWidget::setVR3DVisibile(bool visible)
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	if (!d->VolumeRenderingDisplayNode)
	{
		return;
	}
	d->VolumeRenderingDisplayNode->SetVisibility(visible);
}


void qSlicerVolumeDisplayControllerModuleWidget::setSliceAxialVisible( bool visible )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)
		return;

	if (d->SliceNode[0])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[0]->GetLayoutName());
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->StartSliceNodeInteraction(vtkMRMLSliceNode::SliceVisibleFlag);
		d->SliceNode[0]->SetSliceVisible(visible);
		logic->EndSliceNodeInteraction();
	}

}

void qSlicerVolumeDisplayControllerModuleWidget::setSliceSagittalVisible( bool visible )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)
		return;

	if (d->SliceNode[1])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[0]->GetLayoutName());
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->StartSliceNodeInteraction(vtkMRMLSliceNode::SliceVisibleFlag);
		d->SliceNode[1]->SetSliceVisible(visible);
		logic->EndSliceNodeInteraction();
	}
}

void qSlicerVolumeDisplayControllerModuleWidget::setSliceCoronalVisible( bool visible )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)
		return;

	if (d->SliceNode[2])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[0]->GetLayoutName());
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->StartSliceNodeInteraction(vtkMRMLSliceNode::SliceVisibleFlag);
		d->SliceNode[2]->SetSliceVisible(visible);
		logic->EndSliceNodeInteraction();
	}
}