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
	void setUltrasoundToolDataValid(bool valid);
	void importDataAndEvents();
	void UpdateSliceByLocator();
	bool StartTrackSlice();
	void StopTrackSlice();

	void SetRTImageShiftX(int);
	void SetRTImageShiftY(int);
	void SetRTImageShiftZ(int);
	void SetRTImageRotateX(int);
	void SetRTImageRotateY(int);
	void SetRTImageRotateZ(int);
	void SetImageFusionOpacity(int);

	void on_UltrasoundToolButton_clicked();
	void on_DisplayUSButton_clicked();
	void on_DisplayCTButton_clicked();
	void on_DisplayFusionButton_clicked();

protected:
	QScopedPointer<stkUltrasoundImageFusionWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkUltrasoundImageFusionWidget);
	Q_DISABLE_COPY(stkUltrasoundImageFusionWidget);

};

