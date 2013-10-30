/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include "qSlicerWidget.h"

class stkUltrasoundImageFusionWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkUltrasoundImageFusionWidget : public qSlicerWidget
{
	Q_OBJECT

public:
	typedef qSlicerWidget Superclass;
	stkUltrasoundImageFusionWidget(QWidget *parent = 0);
	~stkUltrasoundImageFusionWidget();

	void StartIGTLImageServer();

public slots:
	void importDataAndEvents();

	void SetRTImageShiftX(int);
	void SetRTImageShiftY(int);
	void SetRTImageShiftZ(int);
	void SetRTImageRotateX(int);
	void SetRTImageRotateY(int);
	void SetRTImageRotateZ(int);

protected:
	QScopedPointer<stkUltrasoundImageFusionWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkUltrasoundImageFusionWidget);
	Q_DISABLE_COPY(stkUltrasoundImageFusionWidget);

};

