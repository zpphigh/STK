/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

#include "qSlicerWidget.h"

class stkSlicerLayoutWidgetPrivate;
class STK_SLICEREXT_EXPORT stkSlicerLayoutWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkSlicerLayoutWidget(QWidget *parent = 0);
	~stkSlicerLayoutWidget();

protected slots:

	void on_SlicerLayoutConventionalViewButton_clicked();
	void on_SlicerLayoutConventionalWidescreenViewButton_clicked();
	void on_SlicerLayoutFourUpViewButton_clicked();
	void on_SlicerLayoutOneUp3DViewButton_clicked();
	void on_SlicerLayoutOneUpRedSliceViewButton_clicked();
	void on_SlicerLayoutOneUpYellowSliceViewButton_clicked();
	void on_SlicerLayoutOneUpGreenSliceViewButton_clicked();
	void on_SlicerLayoutDual3DViewButton_clicked();

protected:
	QScopedPointer<stkSlicerLayoutWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerLayoutWidget);
	Q_DISABLE_COPY(stkSlicerLayoutWidget);
};
