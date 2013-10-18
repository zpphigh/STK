/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include <QWidget>

class stkPolarisPivotCalibrationWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkPolarisPivotCalibrationWidget : public QWidget
{
	Q_OBJECT

public:
	stkPolarisPivotCalibrationWidget(QWidget *parent = 0);
	~stkPolarisPivotCalibrationWidget();

protected slots: 
	void on_initializeButton_clicked();
	void on_saveResultButton_clicked();
	void onPulseTimerout();

protected:
	QScopedPointer<stkPolarisPivotCalibrationWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkPolarisPivotCalibrationWidget);
	Q_DISABLE_COPY(stkPolarisPivotCalibrationWidget);

};
