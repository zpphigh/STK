#pragma once

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerVolumeDisplayControllerModuleExport.h"

class qSlicerVolumeDisplayControllerModuleWidgetPrivate;
class vtkMRMLScalarVolumeNode;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_VOLUMEDISPLAYCONTROLLER_EXPORT qSlicerVolumeDisplayControllerModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVolumeDisplayControllerModuleWidget(QWidget *parent=0);
  virtual ~qSlicerVolumeDisplayControllerModuleWidget();

public slots:

  void setMRMLVolumeNode(vtkMRMLScalarVolumeNode* volumeNode);

protected slots:


protected:
  QScopedPointer<qSlicerVolumeDisplayControllerModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerVolumeDisplayControllerModuleWidget);
  Q_DISABLE_COPY(qSlicerVolumeDisplayControllerModuleWidget);
};

