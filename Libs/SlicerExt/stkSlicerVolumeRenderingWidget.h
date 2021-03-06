/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

#include "qSlicerWidget.h"

class vtkMRMLNode;
class vtkMRMLVolumeRenderingDisplayNode;
class stkSlicerVolumeRenderingWidgetPrivate;
class STK_SLICEREXT_EXPORT stkSlicerVolumeRenderingWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkSlicerVolumeRenderingWidget(QWidget *parent = 0);
	~stkSlicerVolumeRenderingWidget();

public slots:
	/// Set the MRML node of interest
	void setMRMLVolumeNode(vtkMRMLNode* node);
	void setVolumeRenderingVisibile(bool);

	void onCurrentMRMLVolumePropertyNodeChanged( vtkMRMLNode* volumePropertyNode);
	void onCurrentMRMLDisplayNodeChanged( vtkMRMLVolumeRenderingDisplayNode* displayNode);
	void updateFromMRMLDisplayNode();

	void on_resetFocalPointPushButton_clicked();
	void setVR3DVisibile(bool visible);
	void applyPreset( vtkMRMLNode* node );
	void onCropToggled(bool);
	void onROICropDisplayCheckBoxToggled(bool);

protected:
	QScopedPointer<stkSlicerVolumeRenderingWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerVolumeRenderingWidget);
	Q_DISABLE_COPY(stkSlicerVolumeRenderingWidget);
};

