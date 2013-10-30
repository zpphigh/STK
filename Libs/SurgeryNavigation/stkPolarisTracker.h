/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "stkTracker.h"
#include "igstkTracker.h"

class stkTrackerTool;
class stkPolarisTrackerPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkPolarisTracker : public stkTracker
{
	Q_OBJECT

	Q_PROPERTY(int comPortNum READ comPortNum WRITE setComPortNum)

public:
	stkPolarisTracker(QObject *parent = 0);
	~stkPolarisTracker();

	// open communication with the tracking device.
	// It returns true if successful, or false if not successful. 
	virtual bool Open();

	// closes communication with the device. 
	// It returns true if successful, or false if not successful. 
	virtual bool Close();

	// bring the tracker to some defined default state. 
	virtual bool Reset();

	// readies the tracker for tracking the tools connected to the tracker.
	virtual void StartTracking();

	// stops tracker from tracking the tools. 
	virtual void StopTracking();



	// defines the frequency at which the Transform information will be queried from the Tracker device.
	// Note that Tracker devices have their own internal frequency rate, and if you set here
	// a frequency that is higher than what the Tracker device is capable to
	// follow, then you will start receiving transforms with repeated values. 
	virtual void SetFrequency( double frequencyInHz );

	int comPortNum() const;
	void setComPortNum(int num);

	virtual stkTrackerTool* AttachTrackerTool(QString name, QString romFile);
	virtual void AttachTrackerTool5D(QString name, QString portNumber, QString channelNumber){}
	virtual bool DetachTrackerTool(QString name);
	virtual bool DetachAllTools();
	virtual stkTrackerTool* GetTrackerTool(QString name);

public slots:
	virtual void TrackAndSendData();

protected:
	QScopedPointer<stkPolarisTrackerPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkPolarisTracker);
	Q_DISABLE_COPY(stkPolarisTracker);
	
};

