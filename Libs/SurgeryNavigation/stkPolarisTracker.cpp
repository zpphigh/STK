
#include <QHash>

#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"
#include "stkTrackingErrorObserver.h"

#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkSerialCommunication.h"

#include "igstkTransform.h"
#include "igstkTransformObserver.h"
#include "igtlPositionMessage.h"

typedef igstk::Transform            TransformType;
typedef ::itk::Vector<double, 3>    VectorType;
typedef ::itk::Versor<double>       VersorType;


class stkPolarisTrackerPrivate 
{
public:
	static igstk::SerialCommunication::ParityType PARITY;
	static igstk::SerialCommunication::BaudRateType BAUD_RATE;
	static igstk::SerialCommunication::DataBitsType DATA_BITS;
	static igstk::SerialCommunication::StopBitsType STOP_BITS;
	static igstk::SerialCommunication::HandshakeType HAND_SHAKE;

	igstk::PolarisTracker::Pointer Tracker;
	QHash<QString, stkPolarisTrackerTool*> TrackerTools;

	igstk::SerialCommunication::Pointer serialCommunication;
	stkTrackingErrorObserver::Pointer errorObserver;
	bool initialized;

	igtl::PositionMessage::Pointer message;

	igstk::SerialCommunication::PortNumberType ComPortNum;	
};

igstk::SerialCommunication::ParityType stkPolarisTrackerPrivate::PARITY = igstk::SerialCommunication::NoParity;
igstk::SerialCommunication::BaudRateType stkPolarisTrackerPrivate::BAUD_RATE = igstk::SerialCommunication::BaudRate115200;
igstk::SerialCommunication::DataBitsType stkPolarisTrackerPrivate::DATA_BITS = igstk::SerialCommunication::DataBits8;
igstk::SerialCommunication::StopBitsType stkPolarisTrackerPrivate::STOP_BITS = igstk::SerialCommunication::StopBits1;
igstk::SerialCommunication::HandshakeType stkPolarisTrackerPrivate::HAND_SHAKE = igstk::SerialCommunication::HandshakeOff;

stkPolarisTracker::stkPolarisTracker(QObject *parent)
	: stkTracker(parent),d_ptr(new stkPolarisTrackerPrivate)
{
	Q_D(stkPolarisTracker);

	d->initialized = false;
	d->message = igtl::PositionMessage::New();
	//create error observer
	d->errorObserver = stkTrackingErrorObserver::New();

	//create serial communication
	d->serialCommunication = igstk::SerialCommunication::New();

	//observe errors generated by the serial communication
	d->serialCommunication->AddObserver( igstk::OpenPortErrorEvent(),	d->errorObserver );
	d->serialCommunication->AddObserver( igstk::ClosePortErrorEvent(),	d->errorObserver );

	//create tracker
	d->Tracker = igstk::PolarisTracker::New();

	//observe errors generated by the tracker
	d->Tracker->AddObserver( igstk::TrackerOpenErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerInitializeErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerStartTrackingErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerStopTrackingErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerCloseErrorEvent(), d->errorObserver );
	d->Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), d->errorObserver );
}

stkPolarisTracker::~stkPolarisTracker()
{
	Close();
}

int stkPolarisTracker::comPortNum() const
{
	return d_ptr->ComPortNum;
}

void stkPolarisTracker::setComPortNum(int num)
{
	Q_D(stkPolarisTracker);
	d->ComPortNum = static_cast<igstk::SerialCommunication::PortNumberType> (num);
}


// open communication with the tracking device.
// It returns true if successful, or false if not successful. 
bool stkPolarisTracker::Open()
{
	Q_D(stkPolarisTracker);

	igstk::RealTimeClock::Initialize();

	//if already initialized, shutdown everything and reinitialize
	if( d->initialized )
		Close();

	d->serialCommunication->SetPortNumber( d->ComPortNum );
	d->serialCommunication->SetParity( d->PARITY );
	d->serialCommunication->SetBaudRate( d->BAUD_RATE );
	d->serialCommunication->SetDataBits( d->DATA_BITS );
	d->serialCommunication->SetStopBits( d->STOP_BITS );
	d->serialCommunication->SetHardwareHandshake( d->HAND_SHAKE );

	//open serial communication
	d->serialCommunication->OpenCommunication();
	if( d->errorObserver->Error() )
	{
		d->errorObserver->ClearError();
		return false;
	}
	d->Tracker->SetCommunication( d->serialCommunication );

	SetFrequency(30);

	//open tracker communication
	d->Tracker->RequestOpen();
	if( d->errorObserver->Error() )
	{
		d->errorObserver->ClearError();
		return false;
	}

	d->initialized = true;

	return true;
}

// closes communication with the device. 
// It returns true if successful, or false if not successful. 
bool stkPolarisTracker::Close()
{
	Q_D(stkPolarisTracker);
	
	if( d->initialized )
	{
		d->initialized = false;
		d->Tracker->RequestStopTracking();
		if( d->errorObserver->Error() )
		{
			d->errorObserver->ClearError();
			return false;
		}
		d->Tracker->RequestClose();
		if( d->errorObserver->Error() )
		{
			d->errorObserver->ClearError();
			return false;
		}
		d->serialCommunication->CloseCommunication();
		if( d->errorObserver->Error() )
		{
			d->errorObserver->ClearError();
			return false;
		}
	}

	return true;
}

// bring the tracker to some defined default state. 
bool stkPolarisTracker::Reset()
{
	return true;
}

// readies the tracker for tracking the tools connected to the tracker.
void stkPolarisTracker::StartTracking()
{
	Q_D(stkPolarisTracker);

	//start tracking
	d->Tracker->RequestStartTracking();
	if( d->errorObserver->Error() )
	{
		d->errorObserver->ClearError();
		return;
	}


	setIsTracking(true);
}

// stops tracker from tracking the tools. 
void stkPolarisTracker::StopTracking()
{
	Q_D(stkPolarisTracker);
	if(!this->isTracking())
		return;

	d->Tracker->RequestStopTracking();
	setIsTracking(false);
}


// defines the frequency at which the Transform information will be queried from the Tracker device.
// Note that Tracker devices have their own internal frequency rate, and if you set here
// a frequency that is higher than what the Tracker device is capable to
// follow, then you will start receiving transforms with repeated values. 
void stkPolarisTracker::SetFrequency( double frequencyInHz )
{
	Q_D(stkPolarisTracker);
	d->Tracker->RequestSetFrequency( frequencyInHz );
}


stkTrackerTool* stkPolarisTracker::AttachTrackerTool(QString name,  QString  romFile)
{
	Q_D(stkPolarisTracker);

	d->TrackerTools[name] = new stkPolarisTrackerTool(name,romFile,d->Tracker);
	return d->TrackerTools[name];
}

bool stkPolarisTracker::DetachTrackerTool(QString name)
{
	Q_D(stkPolarisTracker);
	if(this->isTracking())
		return false; //先停止跟踪再Detach

	stkPolarisTrackerTool* tool = d->TrackerTools[name];
	if(tool == NULL)
		return false; //没有这个tool

	tool->DetachFromTracker();

	d->TrackerTools.remove(name);

	return true;
}

bool stkPolarisTracker::DetachAllTools()
{
	Q_D(stkPolarisTracker);

	foreach(const QString &name, d->TrackerTools.keys())
	{
		stkPolarisTrackerTool* tool = d->TrackerTools[name];
		if(tool)
			tool->DetachFromTracker();
	}

	d->TrackerTools.clear();

	return true;
}

stkTrackerTool* stkPolarisTracker::GetTrackerTool(QString name)
{
	Q_D(stkPolarisTracker);

	return d->TrackerTools[name];
}


void stkPolarisTracker::TrackAndSendData()
{
	Q_D(stkPolarisTracker);

	igstk::PulseGenerator::CheckTimeouts();
	float pos[3];
	float quat[4];

	foreach( stkPolarisTrackerTool* tool, d->TrackerTools)
	{
		if( tool->FetchToolPosition())
		{
			tool->GetPosition(pos,quat);
			d->message->SetDeviceName(tool->name().toAscii().data());
			d->message->SetPosition(pos);
			d->message->SetQuaternion(quat);
			d->message->Pack();
			this->SendData(d->message->GetPackPointer(),d->message->GetPackSize());
		}
	}
}