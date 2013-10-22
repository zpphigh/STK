
#include "stkAuroraTracker.h"
#include "stkAuroraTrackerTool.h"
#include "igstkAuroraTracker.h"


#include "igstkTransform.h"
#include "igstkTransformObserver.h"
#include "igtlPositionMessage.h"
#include "igstkPulseGenerator.h"

typedef igstk::Transform            TransformType;
typedef ::itk::Vector<double, 3>    VectorType;
typedef ::itk::Versor<double>       VersorType;


class stkAuroraTrackerToolPrivate 
{
public:
	igstk::AuroraTrackerTool::Pointer trackerTool;
	
	igstk::Transform offsetTransform;

	igtl::PositionMessage::Pointer message;	
	igstk::TransformObserver::Pointer coordSystemAObserver;

	float position[3];
	float quaternion[4];
};

stkAuroraTrackerTool::stkAuroraTrackerTool(QString _name,  int toolDof ,int portNumber, int channelNumber, igstk::AuroraTracker::Pointer tracker)
	: stkTrackerTool(NULL),d_ptr(new stkAuroraTrackerToolPrivate)
{
	Q_D(stkAuroraTrackerTool);

	setName(_name);

	//d->offsetTransform.SetToIdentity(1000*60*60*24*365);

	d->trackerTool = igstk::AuroraTrackerTool::New();

	if( toolDof == 5)
	{
		d->trackerTool->RequestSelect6DOFTrackerTool();
		d->trackerTool->RequestSetChannelNumber(channelNumber);	// as we use a 5 DOF we need to set a channel number [0-1]
	}
	else
	{
		d->trackerTool->RequestSelect6DOFTrackerTool();
	}

	d->trackerTool->RequestSetPortNumber(portNumber);
	d->trackerTool->RequestConfigure();
	d->trackerTool->RequestAttachToTracker( tracker );

	d->coordSystemAObserver = igstk::TransformObserver::New();
	d->coordSystemAObserver->ObserveTransformEventsFrom( d->trackerTool );

	d->message = igtl::PositionMessage::New();
	d->message->SetDeviceName(name().toAscii().data());
}

stkAuroraTrackerTool::~stkAuroraTrackerTool()
{

}

void stkAuroraTrackerTool::DetachFromTracker()
{
	Q_D(stkAuroraTrackerTool);

	d->trackerTool->RequestDetachFromTracker();
}


bool stkAuroraTrackerTool::FetchToolPosition()
{
	Q_D(stkAuroraTrackerTool);

	TransformType             sensorTransform;
	igstk::Transform::VectorType pos;
	igstk::Transform::VersorType rot;

	d->coordSystemAObserver->Clear();
	d->trackerTool->RequestGetTransformToParent();
	if (d->coordSystemAObserver->GotTransform())
	{
		sensorTransform = d->coordSystemAObserver->GetTransform();
		if ( sensorTransform.IsValidNow() ) 
		{
			TransformType toolTrans = TransformType::TransformCompose(sensorTransform,d->offsetTransform);

			pos = toolTrans.GetTranslation();
			rot = toolTrans.GetRotation();

			d->message->SetPosition(pos[0], pos[1], pos[2]);
			d->message->SetQuaternion(rot.GetX(), rot.GetY(),rot.GetZ(), rot.GetW());
			d->message->Pack();

			d->message->GetPosition(d->position);
			d->message->GetQuaternion(d->quaternion);

			setDataValid(true);

			return true;
		}
		else
		{
			setDataValid(false);
			return false;
		}
	}

	setDataValid(false);
	return false;
}

bool stkAuroraTrackerTool::GetPosition(float* pos, float* quat)
{
	Q_D(stkAuroraTrackerTool);
	if( !dataValid())
		return false;

	pos[0] = d->position[0];
	pos[1] = d->position[1];
	pos[2] = d->position[2];

	quat[0] = d->quaternion[0];
	quat[1] = d->quaternion[1];
	quat[2] = d->quaternion[2];
	quat[3] = d->quaternion[3];

	return true;
}

void stkAuroraTrackerTool::SetSensorOffset(float x, float y, float z)
{
	Q_D(stkAuroraTrackerTool);
	igstk::Transform::VectorType  translation;
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;

	d->offsetTransform.SetTranslation( translation,0, 1000*60*60*24*365);
}