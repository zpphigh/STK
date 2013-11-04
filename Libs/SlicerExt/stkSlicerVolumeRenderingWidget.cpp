#include "stkSlicerVolumeRenderingWidget.h"
#include "ui_stkSlicerVolumeRenderingWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkSlicerVolumeRenderingLogic.h"
#include "stkSlicerVolumeNodeDisplayHelper.h"
#include "stkSlicerDisplayHelper.h"
#include "vtkMRMLVolumePropertyNode.h"
#include "vtkMRMLAnnotationROINode.h"

class stkSlicerVolumeRenderingWidgetPrivate : public Ui_stkSlicerVolumeRenderingWidget
{
public:
	vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};


stkSlicerVolumeRenderingWidget::stkSlicerVolumeRenderingWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerVolumeRenderingWidgetPrivate)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	d->setupUi(this);

	d->Visibility3DCheckBox->setEnabled(false);
	d->PresetsNodeComboBox->setEnabled(false);
	d->resetFocalPointPushButton->setEnabled(false);
	d->ROICropCheckBox->setEnabled(false);
	d->ROICropDisplayCheckBox->setEnabled(false);

	d->VolumeRenderingDisplayNode = 0;
	connect(d->Visibility3DCheckBox,  SIGNAL(toggled(bool)), this, SLOT(setVolumeRenderingVisibile(bool)));
	connect(d->PresetsNodeComboBox,SIGNAL(currentNodeChanged(vtkMRMLNode*)),this,SLOT(applyPreset(vtkMRMLNode*)));
	connect(d->ROICropCheckBox,SIGNAL(toggled(bool)),this, SLOT(onCropToggled(bool)));
	connect(d->ROICropDisplayCheckBox, SIGNAL(toggled(bool)),this, SLOT(onROICropDisplayCheckBoxToggled(bool)));


	d->VolumePropertyNodeComboBox->setVisible(false);//default hiding this widget
	d->VolumePropertyNodeComboBox->setMRMLScene(qSlicerApplication::application()->mrmlScene());
	connect(d->VolumePropertyNodeComboBox,SIGNAL(currentNodeChanged(vtkMRMLNode*)),this,SLOT(onCurrentMRMLVolumePropertyNodeChanged(vtkMRMLNode*)));
}

stkSlicerVolumeRenderingWidget::~stkSlicerVolumeRenderingWidget()
{

}


/// Set the MRML node of interest
void stkSlicerVolumeRenderingWidget::setMRMLVolumeNode(vtkMRMLNode* node)
{
	Q_D(stkSlicerVolumeRenderingWidget);

	d->Visibility3DCheckBox->setEnabled(false);
	d->PresetsNodeComboBox->setEnabled(false);
	d->resetFocalPointPushButton->setEnabled(false);
	d->ROICropCheckBox->setEnabled(false);
	d->ROICropDisplayCheckBox->setEnabled(false);

	vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
	if(!volumeNode)
	{
		d->VolumeRenderingDisplayNode = 0;
		return;
	}


	//VolumeRendering 
	vtkSlicerVolumeRenderingLogic *volumeRenderingLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(
		qSlicerCoreApplication::application()->moduleManager()->module("VolumeRendering")->logic());
	if(!volumeRenderingLogic)
		return;

	// see if the volume has any display node for a current viewer
	vtkMRMLVolumeRenderingDisplayNode *dnode =
		volumeRenderingLogic->GetFirstVolumeRenderingDisplayNode(volumeNode);

	if (!qSlicerCoreApplication::application()->mrmlScene()->IsClosing())
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

	onCurrentMRMLDisplayNodeChanged(dnode);

	d->Visibility3DCheckBox->setEnabled(true);
	bool vrVisible = d->VolumeRenderingDisplayNode->GetVisibility();
	d->Visibility3DCheckBox->setChecked(vrVisible);

	d->PresetsNodeComboBox->setEnabled(true);
	d->PresetsNodeComboBox->setMRMLScene(volumeRenderingLogic->GetPresetsScene());
	//d->PresetsNodeComboBox->setCurrentNode(0);

	d->ROICropCheckBox->setEnabled(true);
	d->ROICropDisplayCheckBox->setEnabled(true);
	d->resetFocalPointPushButton->setEnabled(true);

	stkSlicerDisplayHelper::ResetFocalPoint();

}


void stkSlicerVolumeRenderingWidget::setVolumeRenderingVisibile(bool visible)
{

}



void stkSlicerVolumeRenderingWidget::onCurrentMRMLDisplayNodeChanged( vtkMRMLVolumeRenderingDisplayNode* displayNode)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	this->qvtkReconnect(d->VolumeRenderingDisplayNode, displayNode, vtkCommand::ModifiedEvent,this, SLOT(updateFromMRMLDisplayNode()));

	d->VolumeRenderingDisplayNode = displayNode;

	this->updateFromMRMLDisplayNode();
}

void stkSlicerVolumeRenderingWidget::updateFromMRMLDisplayNode()
{
	Q_D(stkSlicerVolumeRenderingWidget);
	if(!d->VolumeRenderingDisplayNode)
		return;

	d->VolumePropertyNodeComboBox->setCurrentNode(d->VolumeRenderingDisplayNode->GetVolumePropertyNode());
}


void stkSlicerVolumeRenderingWidget::onCurrentMRMLVolumePropertyNodeChanged( vtkMRMLNode* volumePropertyNode)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	if (!d->VolumeRenderingDisplayNode)
		return;

	d->VolumeRenderingDisplayNode->SetAndObserveVolumePropertyNodeID(volumePropertyNode ? volumePropertyNode->GetID() : 0);
}


void stkSlicerVolumeRenderingWidget::on_resetFocalPointPushButton_clicked()
{
	stkSlicerDisplayHelper::ResetFocalPoint();
}


// --------------------------------------------------------------------------
void stkSlicerVolumeRenderingWidget::setVR3DVisibile(bool visible)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	if (!d->VolumeRenderingDisplayNode)
		return;

	d->VolumeRenderingDisplayNode->SetVisibility(visible);
}


void stkSlicerVolumeRenderingWidget::applyPreset( vtkMRMLNode* node )
{
	Q_D(stkSlicerVolumeRenderingWidget);

	if(!d->VolumeRenderingDisplayNode)
		return;

	vtkMRMLVolumePropertyNode* presetNode = vtkMRMLVolumePropertyNode::SafeDownCast(node);
	vtkMRMLVolumePropertyNode *volumePropertyNode = d->VolumeRenderingDisplayNode->GetVolumePropertyNode();

	if (!presetNode || !volumePropertyNode)
		return;

	volumePropertyNode->Copy(presetNode);
	d->VolumeRenderingDisplayNode->Modified();
}


void stkSlicerVolumeRenderingWidget::onCropToggled( bool crop)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	if (!d->VolumeRenderingDisplayNode)
		return;

	d->VolumeRenderingDisplayNode->SetCroppingEnabled(crop);
}

void stkSlicerVolumeRenderingWidget::onROICropDisplayCheckBoxToggled( bool toggle )
{
	Q_D(stkSlicerVolumeRenderingWidget);
	// When the display box is visible, it should probably activate the
	// cropping (to follow the "what you see is what you get" pattern).
	if (toggle)
		d->ROICropCheckBox->setChecked(true);

	if (!d->VolumeRenderingDisplayNode)
		return;

	d->VolumeRenderingDisplayNode->GetROINode()->SetDisplayVisibility(toggle);
}
