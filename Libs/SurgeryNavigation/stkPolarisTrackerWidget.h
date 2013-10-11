/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include <QWidget>

class stkPolarisTrackerWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkPolarisTrackerWidget : public QWidget
{
	Q_OBJECT

public:
	stkPolarisTrackerWidget(QWidget *parent = 0);
	~stkPolarisTrackerWidget();
	
protected slots: 
	void on_connectServerButton_clicked();
	void on_openDeviceButton_clicked();
	void on_closeDeviceButton_clicked();
	void on_startTrackingButton_clicked();
	void on_stopTrackingButton_clicked();
	void onPulseTimerout();

protected:
	QScopedPointer<stkPolarisTrackerWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkPolarisTrackerWidget);
	Q_DISABLE_COPY(stkPolarisTrackerWidget);
};
