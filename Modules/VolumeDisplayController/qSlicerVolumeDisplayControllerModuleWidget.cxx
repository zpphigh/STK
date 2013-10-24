
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

#include "qSlicerVolumeDisplayWidget.h"

// STD includes
#include <limits>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeDisplayControllerModuleWidgetPrivate: public Ui_qSlicerVolumeDisplayControllerModuleWidget
{
public:
  qSlicerVolumeDisplayControllerModuleWidgetPrivate();
  vtkMRMLScalarVolumeDisplayNode* CurrentVolumeDisplayNode() const;

  vtkMRMLVolumeNode* CurrentVolumeNode;

};

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidgetPrivate::qSlicerVolumeDisplayControllerModuleWidgetPrivate()
{
	this->CurrentVolumeNode = NULL;
}


vtkMRMLScalarVolumeDisplayNode* qSlicerVolumeDisplayControllerModuleWidgetPrivate::CurrentVolumeDisplayNode() const
{
	return this->CurrentVolumeNode ? vtkMRMLScalarVolumeDisplayNode::SafeDownCast(
		this->CurrentVolumeNode->GetDisplayNode()) : 0;
}




//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidget::qSlicerVolumeDisplayControllerModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerVolumeDisplayControllerModuleWidgetPrivate )
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

  this->Superclass::setup();

  connect(this,SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
	  d->ActiveVolumeNodeSelector,SLOT(setMRMLScene(vtkMRMLScene*)));

   
  QObject::connect(d->ActiveVolumeNodeSelector,SIGNAL(currentNodeChanged(vtkMRMLNode*)),this, SLOT(setMRMLVolumeNode(vtkMRMLNode*)));

  QObject::connect(d->InterpolateCheckbox, SIGNAL(toggled(bool)),
	  this , SLOT(setInterpolate(bool)));

  QObject::connect(d->ColorTableComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this , SLOT(setColorNode(vtkMRMLNode*)));

  QObject::connect(d->CTBonePresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));

  QObject::connect(d->CTAirPresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));

  QObject::connect(d->PETPresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));

  QObject::connect(d->CTAbdomenPresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));

  QObject::connect(d->CTBrainPresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));

  QObject::connect(d->CTLungPresetToolButton, SIGNAL(clicked()),
	  this , SLOT(onPresetButtonClicked()));
}


void qSlicerVolumeDisplayControllerModuleWidget::setMRMLVolumeNode( vtkMRMLScalarVolumeNode* volumeNode )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	vtkMRMLScalarVolumeDisplayNode* oldVolumeDisplayNode = d->CurrentVolumeDisplayNode();

	d->CurrentVolumeNode = volumeNode;

	d->MRMLWindowLevelWidget->setMRMLVolumeNode(volumeNode);
	d->MRMLVolumeThresholdWidget->setMRMLVolumeNode(volumeNode);

	qvtkReconnect(oldVolumeDisplayNode, volumeNode ? volumeNode->GetDisplayNode() :0,
		vtkCommand::ModifiedEvent,
		this, SLOT(updateWidgetFromMRML()));

	this->updateWidgetFromMRML();
}


void qSlicerVolumeDisplayControllerModuleWidget::updateWidgetFromMRML()
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	vtkMRMLScalarVolumeDisplayNode* displayNode = d->CurrentVolumeDisplayNode();
	if (displayNode)
	{
		d->ColorTableComboBox->setCurrentNode(displayNode->GetColorNode());
		d->InterpolateCheckbox->setChecked(displayNode->GetInterpolate());
	}
	//if (this->isVisible())
	//{
	//	this->updateTransferFunction();
	//}
}

//void qSlicerVolumeDisplayControllerModuleWidget::updateTransferFunction()
//{
//
//}

void qSlicerVolumeDisplayControllerModuleWidget::onPresetButtonClicked()
{
	QToolButton* preset = qobject_cast<QToolButton*>(this->sender());
	this->setPreset(preset->accessibleName());
}

void qSlicerVolumeDisplayControllerModuleWidget::setInterpolate( bool interpolate )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	vtkMRMLScalarVolumeDisplayNode* displayNode =	d->CurrentVolumeDisplayNode();
	if (!displayNode)
		return;

	displayNode->SetInterpolate(interpolate);
}

void qSlicerVolumeDisplayControllerModuleWidget::setColorNode( vtkMRMLNode* colorNode )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	vtkMRMLScalarVolumeDisplayNode* displayNode =	d->CurrentVolumeDisplayNode();
	if (!displayNode || !colorNode)
		return;

	Q_ASSERT(vtkMRMLColorNode::SafeDownCast(colorNode));
	displayNode->SetAndObserveColorNodeID(colorNode->GetID());
}

void qSlicerVolumeDisplayControllerModuleWidget::setPreset( const QString& presetName )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);
	QString colorNodeID;
	double window = -1.;
	double level = std::numeric_limits<double>::max();
	if (presetName == "CT-Bone")
	{
		colorNodeID = "vtkMRMLColorTableNodeGrey";
		window = 1000.;
		level = 400.;
	}
	else if (presetName == "CT-Air")
	{
		colorNodeID = "vtkMRMLColorTableNodeGrey";
		window = 1000.;
		level = -426.;
	}
	else if (presetName == "PET")
	{
		colorNodeID = "vtkMRMLColorTableNodeRainbow";
		window = 10000.;
		level = 6000.;
	}
	else if (presetName == "CT-Abdomen")
	{
		colorNodeID = "vtkMRMLColorTableNodeGrey";
		window = 350.;
		level = 40.;
	}
	else if (presetName == "CT-Brain")
	{
		colorNodeID = "vtkMRMLColorTableNodeGrey";
		window = 100.;
		level = 50.;
	}
	else if (presetName == "CT-Lung")
	{
		colorNodeID = "vtkMRMLColorTableNodeGrey";
		window = 1400.;
		level = -500.;
	}

	vtkMRMLNode* colorNode = this->mrmlScene()->GetNodeByID(colorNodeID.toLatin1());
	if (colorNode)
	{
		this->setColorNode(colorNode);
	}
	if (window != -1 || level!= std::numeric_limits<double>::max())
	{
		d->MRMLWindowLevelWidget->setAutoWindowLevel(qMRMLWindowLevelWidget::Manual);
	}
	if (window != -1 && level != std::numeric_limits<double>::max())
	{
		d->MRMLWindowLevelWidget->setWindowLevel(window, level);
	}
	else if (window != -1)
	{
		d->MRMLWindowLevelWidget->setWindow(window);
	}
	else if (level != std::numeric_limits<double>::max())
	{
		d->MRMLWindowLevelWidget->setLevel(level);
	}
}
