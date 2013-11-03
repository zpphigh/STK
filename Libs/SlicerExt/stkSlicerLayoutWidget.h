/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"


#include <QWidget>

class stkSlicerLayoutWidgetPrivate;
class stkSlicerLayoutWidget : public QWidget
{
	Q_OBJECT

public:
	stkSlicerLayoutWidget(QWidget *parent = 0);
	~stkSlicerLayoutWidget();

protected:
	QScopedPointer<stkSlicerLayoutWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerLayoutWidget);
	Q_DISABLE_COPY(stkSlicerLayoutWidget);
};
