/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

#include <QWidget>

class stkSlicerDataWidgetPrivate;
class STK_SLICEREXT_EXPORT stkSlicerDataWidget : public QWidget
{
	Q_OBJECT

public:
	stkSlicerDataWidget(QWidget *parent = 0);
	~stkSlicerDataWidget();

protected:
	QScopedPointer<stkSlicerDataWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerDataWidget);
	Q_DISABLE_COPY(stkSlicerDataWidget);
};
