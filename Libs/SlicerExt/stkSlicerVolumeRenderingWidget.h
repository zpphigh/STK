/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

#include "qSlicerWidget.h"


class stkSlicerVolumeRenderingWidgetPrivate;
class STK_SLICEREXT_EXPORT stkSlicerVolumeRenderingWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkSlicerVolumeRenderingWidget(QWidget *parent = 0);
	~stkSlicerVolumeRenderingWidget();

protected:
	QScopedPointer<stkSlicerVolumeRenderingWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerVolumeRenderingWidget);
	Q_DISABLE_COPY(stkSlicerVolumeRenderingWidget);
};

