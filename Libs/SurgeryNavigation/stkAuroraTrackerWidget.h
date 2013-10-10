/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QWidget>

class stkAuroraTrackerWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkAuroraTrackerWidget : public QWidget
{
	Q_OBJECT

public:
	stkAuroraTrackerWidget(QWidget *parent = 0);
	~stkAuroraTrackerWidget();
	
protected slots: 
	void on_connectServerButton_clicked();
	void on_openDeviceButton_clicked();
	void on_closeDeviceButton_clicked();
	void on_startTrackingButton_clicked();
	void on_stopTrackingButton_clicked();
	void onPulseTimerout();

protected:
	QScopedPointer<stkAuroraTrackerWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkAuroraTrackerWidget);
	Q_DISABLE_COPY(stkAuroraTrackerWidget);
};
