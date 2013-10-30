/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QObject>

#include "igstkTrackerTool.h"

class stkTrackerTool;
class stkTrackerPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkTracker : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool isTracking READ isTracking)
	Q_PROPERTY(bool isOpenned READ isOpenned)
	Q_PROPERTY(bool isServerConnected READ isServerConnected)

public:
	stkTracker(QObject *parent = 0);
	~stkTracker();


	bool isTracking() const;
	bool isOpenned() const;
	bool isServerConnected() const;

	// open communication with the tracking device.
	// It returns true if successful, or false if not successful. 
	virtual bool Open() = 0;

	// closes communication with the device. 
	// It returns true if successful, or false if not successful. 
	virtual bool Close() = 0;

	// bring the tracker to some defined default state. 
	virtual bool Reset() = 0;

	// readies the tracker for tracking the tools connected to the tracker.
	virtual void StartTracking() = 0;

	// stops tracker from tracking the tools. 
	virtual void StopTracking() = 0;

	// defines the frequency at which the Transform information will be queried from the Tracker device.
	// Note that Tracker devices have their own internal frequency rate, and if you set here
	// a frequency that is higher than what the Tracker device is capable to
	// follow, then you will start receiving transforms with repeated values. 
	virtual void SetFrequency( double frequencyInHz ) = 0;


	virtual stkTrackerTool* AttachTrackerTool(QString name, QString options) = 0;
	virtual void AttachTrackerTool5D(QString name, QString options, QString options2) = 0;
	virtual bool DetachTrackerTool(QString name) =0;
	virtual bool DetachAllTools() = 0;
	virtual stkTrackerTool* GetTrackerTool(QString name) = 0;

	bool ConnectServer(QString hostname, int port);
	void DisconnectServer();

	int SendData(const void* data, int length);

public slots:
	//获得一个跟踪数据并发送出去
	virtual void TrackAndSendData() = 0;

protected:
	void DelayNSeconds(int seconds);

	void setIsTracking(bool s);
	void setIsOpenned(bool o);

protected:
	QScopedPointer<stkTrackerPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkTracker);
	Q_DISABLE_COPY(stkTracker);
	
};
