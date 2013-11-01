
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
	bool AbortTracking;
};


stkTrackerThread::stkTrackerThread(QObject *parent)
	: QThread(parent),d_ptr(new stkTrackerThreadPrivate)
{
	Q_D(stkTrackerThread);
	d->TrackerType = TRACKER_TYPE_NONE;
	d->Tracker = NULL;
	d->IGTLServerName = QString("");
	d->IGTLServerPort = 18944;
	d->AbortTracking = true;
}

stkTrackerThread::~stkTrackerThread()
{
	Q_D(stkTrackerThread);
	if(!d->Tracker)
		return;

	d->Tracker->DetachAllTools();
	d->Tracker->Close();
}
void stkTrackerThread::UseTrackerAurora(int comPort)
{
	Q_D(stkTrackerThread);
	d->TrackerType = TRACKER_TYPE_AURORA;
	stkAuroraTracker* tracker = new stkAuroraTracker;
	tracker->setComPortNum(comPort);
	d->Tracker = tracker;
}

void stkTrackerThread::UseTrackerPolaris(int comPort)
{
	Q_D(stkTrackerThread);
	d->TrackerType = TRACKER_TYPE_POLARIS;
	stkPolarisTracker* tracker = new stkPolarisTracker;
	tracker->setComPortNum(comPort);
	d->Tracker = tracker;
}

void stkTrackerThread::SetIGTServer(QString hostname, int port)
{
	Q_D(stkTrackerThread);
	d->IGTLServerName = hostname;
	d->IGTLServerPort = port;
}

bool stkTrackerThread::StartTracking()
{
	Q_D(stkTrackerThread);


	if(d->TrackerType == TRACKER_TYPE_AURORA)
	{
		if(!d->Tracker->Open())
			return false;

		stkTrackerTool* CalibrationTool = d->Tracker->AttachTrackerTool("CalibrationTool", "0");
		if(!CalibrationTool)
			return false;

		d->Tracker->AttachTrackerTool("UltrasoundTool", "1");
		//d->Tracker->AttachTrackerTool("SurgeryTool", "2");
	//		connect(CalibrationTool, SIGNAL(dataValidChanged(bool)),this,SLOT(setCalibrationToolDataValid(bool)));
	}

	//Connect IGT Server
	if( !d->Tracker->isServerConnected())
	{
		if(!d->Tracker->ConnectServer(d->IGTLServerName,d->IGTLServerPort))
			return false;
	}
	//Start Tracking
	if(!d->Tracker->isTracking())
	{
		d->Tracker->StartTracking();
	}

	emit TrackingStarted();

	return true;
}

void stkTrackerThread::StopTracking()
{
	Q_D(stkTrackerThread);

	if(d->Tracker->isTracking())
		d->Tracker->StopTracking();
	
	emit TrackingStoped();
}

void stkTrackerThread::run()
{
	Q_D(stkTrackerThread);
	d->AbortTracking = false;

	StartTracking();

	while(!d->AbortTracking){

		d->Tracker->TrackAndSendData();
		QThread::msleep(30);
	}

	StopTracking();
}

void stkTrackerThread::TrackAndSendData()
{
	Q_D(stkTrackerThread);

	d->Tracker->TrackAndSendData();
}


void stkTrackerThread::AbortTracking()
{
	Q_D(stkTrackerThread);
	d->AbortTracking = true;
}