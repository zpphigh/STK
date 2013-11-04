#include "stkSlicerSliceNodeWidget.h"
#include "ui_stkSlicerSliceNodeWidget.h"
#include "vtkMRMLSliceNode.h"
#include "qMRMLSliceWidget.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLSliceLogic.h"
#include "qSlicerApplication.h"
#include "qMRMLSliceControllerWidget.h"
#include "qSlicerLayoutManager.h"

class stkSlicerSliceNodeWidgetPrivate : public Ui_stkSlicerSliceNodeWidget
{
public:
	vtkMRMLSliceNode* SliceNode[3];	
};


stkSlicerSliceNodeWidget::stkSlicerSliceNodeWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerSliceNodeWidgetPrivate)
{
	Q_D(stkSlicerSliceNodeWidget);
	d->setupUi(this);

	d->SliceNode[0] = 0;
	d->SliceNode[1] = 0;
	d->SliceNode[2] = 0;

	d->AxialSliceSlider->setEnabled(false);
	d->AxialSliceVisibileCheckBox->setEnabled(false);
	d->CoronalSliceSlider->setEnabled(false);
	d->CoronalSliceVisibileCheckBox->setEnabled(false);
	d->SagittalSliceSlider->setEnabled(false);
	d->SagittalSliceVisibileCheckBox->setEnabled(false);

	connect(d->AxialSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceAxialVisible(bool)));
	connect(d->SagittalSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceSagittalVisible(bool)));
	connect(d->CoronalSliceVisibileCheckBox,SIGNAL(toggled(bool)),this,SLOT(setSliceCoronalVisible(bool)));
	connect(d->AxialSliceSlider, SIGNAL(valueChanged(double)),this,SLOT(setAxialSliceOffsetValue(double)));
	connect(d->SagittalSliceSlider,SIGNAL(valueChanged(double)),this,SLOT(setSagittalSliceOffsetValue(double)));
	connect(d->CoronalSliceSlider,SIGNAL(valueChanged(double)),this,SLOT(setCoronalSliceOffsetValue(double)));
}

stkSlicerSliceNodeWidget::~stkSlicerSliceNodeWidget()
{

}


/// Set the MRML node of interest
void stkSlicerSliceNodeWidget::setMRMLVolumeNode(vtkMRMLNode* node)
{
	Q_D(stkSlicerSliceNodeWidget);

	d->AxialSliceSlider->setEnabled(false);
	d->AxialSliceVisibileCheckBox->setEnabled(false);
	d->CoronalSliceSlider->setEnabled(false);
	d->CoronalSliceVisibileCheckBox->setEnabled(false);
	d->SagittalSliceSlider->setEnabled(false);
	d->SagittalSliceVisibileCheckBox->setEnabled(false);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app)
	{
		return;
	}

	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)
	{
		return;
	}

	vtkMRMLScene* scene = app->mrmlScene();
	if(!scene)
	{
		return;
	}

	vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
	if(!volumeNode)
	{
		return;
	}

	d->SliceNode[0] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeRed"));
	d->SliceNode[1] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeYellow"));
	d->SliceNode[2] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeGreen"));

	if (d->SliceNode[0])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[0]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);

		d->AxialSliceSlider->setEnabled(true);
		d->AxialSliceVisibileCheckBox->setEnabled(true);

		d->AxialSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->AxialSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
		d->AxialSliceVisibileCheckBox->setChecked(d->SliceNode[0]->GetSliceVisible());
	}
	if (d->SliceNode[1])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[1]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);

		d->CoronalSliceSlider->setEnabled(true);
		d->CoronalSliceVisibileCheckBox->setEnabled(true);
		d->SagittalSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->SagittalSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
		d->CoronalSliceVisibileCheckBox->setChecked(d->SliceNode[1]->GetSliceVisible());
	}
	if (d->SliceNode[2])
	{
		qMRMLSliceWidget *sliceWidget = layoutManager->sliceWidget(d->SliceNode[2]->GetLayoutName());
		double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
		vtkMRMLSliceLogic* logic = sliceWidget->sliceController()->sliceLogic();
		logic->GetLowestVolumeSliceBounds(sliceBounds);
		Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);

		d->SagittalSliceSlider->setEnabled(true);
		d->SagittalSliceVisibileCheckBox->setEnabled(true);

		d->CoronalSliceSlider->setRange(sliceBounds[4], sliceBounds[5]);
		d->CoronalSliceSlider->setValue((sliceBounds[4]+sliceBounds[5])/2);
		d->SagittalSliceVisibileCheckBox->setChecked(d->SliceNode[0]->GetSliceVisible());
	}

}


void stkSlicerSliceNodeWidget::setAxialSliceOffsetValue( double newValue)
{
	Q_D(stkSlicerSliceNodeWidget);

	d->SliceNode[0]->SetSliceOffset(newValue);	
}

void stkSlicerSliceNodeWidget::setSagittalSliceOffsetValue( double newValue)
{
	Q_D(stkSlicerSliceNodeWidget);

	d->SliceNode[1]->SetSliceOffset(newValue);	
}

void stkSlicerSliceNodeWidget::setCoronalSliceOffsetValue( double newValue)
{
	Q_D(stkSlicerSliceNodeWidget);

	d->SliceNode[2]->SetSliceOffset(newValue);	
}



void stkSlicerSliceNodeWidget::setSliceAxialVisible( bool visible )
{
	Q_D(stkSlicerSliceNodeWidget);
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

void stkSlicerSliceNodeWidget::setSliceSagittalVisible( bool visible )
{
	Q_D(stkSlicerSliceNodeWidget);
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

void stkSlicerSliceNodeWidget::setSliceCoronalVisible( bool visible )
{
	Q_D(stkSlicerSliceNodeWidget);
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
