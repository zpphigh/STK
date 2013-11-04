
#include "stkTrackerThread.h"
#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"
#include "stkAuroraTracker.h"
#include "stkAuroraTrackerTool.h"

enum TrackerType{
	TRACKER_TYPE_NONE		= 0,
	TRACKER_TYPE_ASCENSION  = 1,
	TRACKER_TYPE_POLARIS	= 2,
	TRACKER_TYPE_AURORA		= 3
};


class stkTrackerThreadPrivate
{
public:
	int TrackerType;
	stkTracker* Tracker;

	QString IGTLServerName;
	int IGTLServerPort;
	bool Abort;


	bool startTracking();
	void stopTracking();
};


stkTrackerThread::stkTrackerThread(QObject *parent)
	: QThread(parent),d_ptr(new stkTrackerThreadPrivate)
{
	Q_D(stkTrackerThread);
	d->TrackerType = TRACKER_TYPE_NONE;
	d->Tracker = NULL;
	d->IGTLServerName = QString("");
	d->IGTLServerPort = 18944;
	d->Abort = true;
}

stkTrackerThread::~stkTrackerThread()
{
	Q_D(stkTrackerThread);
	if(!d->Tracker)
		return;

	d->Tracker->DetachAllTools();
	d->Tracker->Close();
}

void stkTrackerThread::InitAuroraTracker(int comPort)
{
	Q_D(stkTrackerThread);
	d->TrackerType = TRACKER_TYPE_AURORA;
	stkAuroraTracker* tracker = new stkAuroraTracker;
	tracker->setComPortNum(comPort);
	d->Tracker = tracker;
}

stkTrackerTool* stkTrackerThread::GetTrackerTool(QString name)
{
	Q_D(stkTrackerThread);
	return d->Tracker->GetTrackerTool(name);
}

void stkTrackerThread::SetIGTServer(QString hostname, int port)
{
	Q_D(stkTrackerThread);
	d->IGTLServerName = hostname;
	d->IGTLServerPort = port;
}

bool stkTrackerThreadPrivate::startTracking()
{
	if(TrackerType == TRACKER_TYPE_AURORA)
	{
		if(!Tracker->Open())
			return false;

		Tracker->AttachTrackerTool("CalibrationTool", "0");
		Tracker->AttachTrackerTool("UltrasoundTool", "1");
	}

	//Connect IGT Server
	if( !Tracker->isServerConnected())
	{
		if(!Tracker->ConnectServer(IGTLServerName,IGTLServerPort))
			return false;
	}
	//Start Tracking
	if(!Tracker->isTracking())
	{
		Tracker->StartTracking();
	}

	return true;
}

void stkTrackerThreadPrivate::stopTracking()
{
	if(Tracker->isTracking())
		Tracker->StopTracking();	
}

void stkTrackerThread::run()
{
	Q_D(stkTrackerThread);
	d->Abort = false;

	if(!d->startTracking())
		return;

	emit TrackingStarted();

	while(!d->Abort){

		d->Tracker->TrackAndSendData();
		QThread::msleep(30);
	}

	d->stopTracking();
}

void stkTrackerThread::Abort()
{
	Q_D(stkTrackerThread);
	d->Abort = true;
}