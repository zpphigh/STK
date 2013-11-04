/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

#include "stkSlicerExtExport.h"
#include "qSlicerWidget.h"

class vtkMRMLNode;
class stkSlicerSliceNodeWidgetPrivate;
class stkSlicerSliceNodeWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkSlicerSliceNodeWidget(QWidget *parent = 0);
	~stkSlicerSliceNodeWidget();

public slots:
	void setMRMLVolumeNode(vtkMRMLNode* node);
	void setAxialSliceOffsetValue(double );
	void setSagittalSliceOffsetValue(double);
	void setCoronalSliceOffsetValue(double);
	void setSliceAxialVisible(bool);
	void setSliceSagittalVisible(bool);
	void setSliceCoronalVisible(bool);

protected:
	QScopedPointer<stkSlicerSliceNodeWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerSliceNodeWidget);
	Q_DISABLE_COPY(stkSlicerSliceNodeWidget);
};

