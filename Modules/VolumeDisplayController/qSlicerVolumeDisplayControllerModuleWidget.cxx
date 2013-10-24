
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
#include "vtkMRMLVolumeRenderingDisplayNode.h"

// STD includes
#include <limits>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeDisplayControllerModuleWidgetPrivate: public Ui_qSlicerVolumeDisplayControllerModuleWidget
{
public:
  qSlicerVolumeDisplayControllerModuleWidgetPrivate();

  vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModuleWidgetPrivate::qSlicerVolumeDisplayControllerModuleWidgetPrivate()
{
	VolumeRenderingDisplayNode = 0;
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

  QObject::connect(d->ActiveVolumeNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(setMRMLVolumeNode(vtkMRMLNode*)));
   
}


void qSlicerVolumeDisplayControllerModuleWidget::setMRMLVolumeNode( vtkMRMLNode* volumeNode )
{
	Q_D(qSlicerVolumeDisplayControllerModuleWidget);

	d->VolumeDisplayWidget->setMRMLVolumeNode(volumeNode);
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
