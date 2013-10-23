#pragma once

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerVolumeDisplayControllerModuleExport.h"

class qSlicerVolumeDisplayControllerModulePrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_VOLUMEDISPLAYCONTROLLER_EXPORT
qSlicerVolumeDisplayControllerModule
  : public qSlicerLoadableModule
{
  Q_OBJECT
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerVolumeDisplayControllerModule(QObject *parent=0);
  virtual ~qSlicerVolumeDisplayControllerModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  virtual QIcon icon()const;

  virtual QStringList categories()const;
  virtual QStringList dependencies() const;

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerVolumeDisplayControllerModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerVolumeDisplayControllerModule);
  Q_DISABLE_COPY(qSlicerVolumeDisplayControllerModule);

};

