/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QThread>

class stkTrackerThreadPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkTrackerThread : public QThread
{
	Q_OBJECT

public:
	stkTrackerThread(QObject *parent = 0);
	~stkTrackerThread();

	void UseTrackerAurora(int comPort);
	void UseTrackerPolaris(int comPort);
	void SetIGTServer(QString hostname, int port);
	
	bool StartTracking();
	void StopTracking();

signals:
	void TrackingStarted();
	void TrackingStoped();

public slots:
	
	void TrackAndSendData();

protected:
	void run();

protected:
	QScopedPointer<stkTrackerThreadPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkTrackerThread);
	Q_DISABLE_COPY(stkTrackerThread);	
};
