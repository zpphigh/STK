/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QThread>

class stkTrackerTool;
class stkTrackerThreadPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkTrackerThread : public QThread
{
	Q_OBJECT

public:
	stkTrackerThread(QObject *parent = 0);
	~stkTrackerThread();

	void InitAuroraTracker(int comPort);
	void SetIGTServer(QString hostname, int port);
	stkTrackerTool* GetTrackerTool(QString name);
	void Abort();

signals:
	void TrackingStarted();
	void TrackingStoped();

protected:
	void run();
	bool startTracking();
	void stopTracking();

protected:
	QScopedPointer<stkTrackerThreadPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkTrackerThread);
	Q_DISABLE_COPY(stkTrackerThread);	
};
