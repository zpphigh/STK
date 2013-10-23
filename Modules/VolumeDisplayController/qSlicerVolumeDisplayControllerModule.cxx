// Qt includes
#include <QtPlugin>

// VolumeDisplayController Logic includes
#include <vtkSlicerVolumeDisplayControllerLogic.h>

// VolumeDisplayController includes
#include "qSlicerVolumeDisplayControllerModule.h"
#include "qSlicerVolumeDisplayControllerModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVolumeDisplayControllerModule, qSlicerVolumeDisplayControllerModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeDisplayControllerModulePrivate
{
public:
  qSlicerVolumeDisplayControllerModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModulePrivate
::qSlicerVolumeDisplayControllerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModule methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModule
::qSlicerVolumeDisplayControllerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVolumeDisplayControllerModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModule::~qSlicerVolumeDisplayControllerModule()
{
}

//-----------------------------------------------------------------------------
QIcon qSlicerVolumeDisplayControllerModule::icon()const
{
  return QIcon(":/Icons/VolumeDisplayController.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeDisplayControllerModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeDisplayControllerModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVolumeDisplayControllerModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerVolumeDisplayControllerModule
::createWidgetRepresentation()
{
  return new qSlicerVolumeDisplayControllerModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVolumeDisplayControllerModule::createLogic()
{
  return vtkSlicerVolumeDisplayControllerLogic::New();
}
