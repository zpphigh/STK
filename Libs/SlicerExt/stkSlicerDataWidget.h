/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

#include "qSlicerWidget.h"

class stkSlicerDataWidgetPrivate;
class STK_SLICEREXT_EXPORT stkSlicerDataWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkSlicerDataWidget(QWidget *parent = 0);
	~stkSlicerDataWidget();

	void on_LoadSceneButton_clicked();
	void on_SaveSceneButton_clicked();
	void on_CloseSceneButton_clicked();
	void on_SlicerSettingsButton_clicked();

protected:
	QScopedPointer<stkSlicerDataWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerDataWidget);
	Q_DISABLE_COPY(stkSlicerDataWidget);
};
