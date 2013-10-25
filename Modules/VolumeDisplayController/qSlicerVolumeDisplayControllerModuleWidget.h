#pragma once

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerVolumeDisplayControllerModuleExport.h"



class vtkMRMLNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLVolumeRenderingDisplayNode;


class qSlicerVolumeDisplayControllerModuleWidgetPrivate;
/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_VOLUMEDISPLAYCONTROLLER_EXPORT qSlicerVolumeDisplayControllerModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVolumeDisplayControllerModuleWidget(QWidget *parent=0);
  virtual ~qSlicerVolumeDisplayControllerModuleWidget();

  vtkMRMLScalarVolumeNode* mrmlVolumeNode()const;
  vtkMRMLVolumeRenderingDisplayNode* mrmlVolumeRenderingDisplayNode() const;

public slots:

  void setMRMLVolumeNode(vtkMRMLNode* node);

  void setAxialSliceOffsetValue(double );
  void setSagittalSliceOffsetValue(double);
  void setCoronalSliceOffsetValue(double);

  void setVR3DVisibile(bool);
  void setSliceAxialVisible(bool);
  void setSliceSagittalVisible(bool);
  void setSliceCoronalVisible(bool);


protected slots:
  void onCropToggled(bool);
  void onROICropDisplayCheckBoxToggled(bool);
  void applyPreset( vtkMRMLNode* node );
  void onCurrentMRMLVolumePropertyNodeChanged( vtkMRMLNode* volumePropertyNode);
  void on_resetFocalPointPushButton_clicked();

  void onCurrentMRMLDisplayNodeChanged( vtkMRMLVolumeRenderingDisplayNode* displayNode);
  void updateFromMRMLDisplayNode();

protected:
  QScopedPointer<qSlicerVolumeDisplayControllerModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerVolumeDisplayControllerModuleWidget);
  Q_DISABLE_COPY(qSlicerVolumeDisplayControllerModuleWidget);
};

