
#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"
#include "igstkPolarisTracker.h"


#include "igstkTransform.h"
#include "igstkTransformObserver.h"
#include "igtlPositionMessage.h"
#include "igstkPulseGenerator.h"

typedef igstk::Transform            TransformType;
typedef ::itk::Vector<double, 3>    VectorType;
typedef ::itk::Versor<double>       VersorType;


class stkPolarisTrackerToolPrivate 
{
public:
	igstk::PolarisTrackerTool::Pointer trackerTool;
	
	igstk::Transform offsetTransform;

	igtl::PositionMessage::Pointer message;	
	igstk::TransformObserver::Pointer coordSystemAObserver;

	float position[3];
	float quaternion[4];
};

stkPolarisTrackerTool::stkPolarisTrackerTool(QString _name,  QString  romFile, igstk::PolarisTracker::Pointer tracker)
	: stkTrackerTool(NULL),d_ptr(new stkPolarisTrackerToolPrivate)
{
	Q_D(stkPolarisTrackerTool);

	setName(_name);
	d->offsetTransform.SetToIdentity(1000*60*60*24*365);
	d->message = igtl::PositionMessage::New();
	d->message->SetDeviceName(name().toAscii().data());

	d->trackerTool = igstk::PolarisTrackerTool::New();
	d->trackerTool->RequestSelectWirelessTrackerTool();
	d->trackerTool->RequestSetSROMFileName(romFile.toAscii().data());
	d->trackerTool->RequestConfigure();
	d->trackerTool->RequestAttachToTracker( tracker);

	d->coordSystemAObserver = igstk::TransformObserver::New();
	d->coordSystemAObserver->ObserveTransformEventsFrom( d->trackerTool );
}

stkPolarisTrackerTool::~stkPolarisTrackerTool()
{

}

void stkPolarisTrackerTool::DetachFromTracker()
{
	Q_D(stkPolarisTrackerTool);

	d->trackerTool->RequestDetachFromTracker();
}


igstk::TrackerTool::Pointer stkPolarisTrackerTool::GetTrackerToolPointer()
{
	Q_D(stkPolarisTrackerTool);
	return d->trackerTool;
}


bool stkPolarisTrackerTool::FetchToolPosition()
{
	Q_D(stkPolarisTrackerTool);

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

bool stkPolarisTrackerTool::GetPosition(float* pos, float* quat)
{
	Q_D(stkPolarisTrackerTool);
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

void stkPolarisTrackerTool::SetSensorOffset(float x, float y, float z)
{
	Q_D(stkPolarisTrackerTool);
	igstk::Transform::VectorType  translation;
	translation[0] = x;
	translation[1] = y;
	translation[2] = z;

	d->offsetTransform.SetTranslation( translation,0, 1000*60*60*24*365);
}