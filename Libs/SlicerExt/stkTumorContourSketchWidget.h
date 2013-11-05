/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

#include "stkSlicerExtExport.h"
#include "qSlicerWidget.h"

#include "ui_stkTumorContourSketchWidget.h"

class stkTumorContourSketchWidgetPrivate;
class STK_SLICEREXT_EXPORT stkTumorContourSketchWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkTumorContourSketchWidget(QWidget *parent = 0);
	~stkTumorContourSketchWidget();

protected:
	QScopedPointer<stkTumorContourSketchWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkTumorContourSketchWidget);
	Q_DISABLE_COPY(stkTumorContourSketchWidget);
};

