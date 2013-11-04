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

protected:
	QScopedPointer<stkSlicerSliceNodeWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerSliceNodeWidget);
	Q_DISABLE_COPY(stkSlicerSliceNodeWidget);
};

