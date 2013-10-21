#include "stkSurgeryRoute.h"

class stkSurgeryRoutePrivate
{
public:
	double Power;
	int StartTime;
	int EndTime;
	double Rotation[3];
	double Head[3];
	double Tail[3];
	int RouteID;
};

stkSurgeryRoute::stkSurgeryRoute(QObject *parent)
	: QObject(parent), d_ptr(new stkSurgeryRoutePrivate)
{

}

stkSurgeryRoute::~stkSurgeryRoute()
{

}

double stkSurgeryRoute::power() const
{
	return d_ptr->Power;
}

void stkSurgeryRoute::setPower(double v)
{
	Q_D(stkSurgeryRoute);
	d->Power = v;
}

int stkSurgeryRoute::startTime() const
{
	return d_ptr->StartTime;
}

void stkSurgeryRoute::setStartTime(int v)
{
	Q_D(stkSurgeryRoute);
	d->StartTime = v;
}

int stkSurgeryRoute::endTime() const
{
	return d_ptr->EndTime;
}

void stkSurgeryRoute::setEndTime(int v)
{
	Q_D(stkSurgeryRoute);
	d->EndTime = v;
}

double* stkSurgeryRoute::routeHead() const
{
	return d_ptr->Head;
}

void stkSurgeryRoute::setRouteHead(double* v)
{
	Q_D(stkSurgeryRoute);
	d->Head[0] = v[0];
	d->Head[1] = v[1];
	d->Head[2] = v[2];
}

void stkSurgeryRoute::setRouteHead(double x, double y, double z)
{
	Q_D(stkSurgeryRoute);
	d->Head[0] = x;
	d->Head[1] = y;
	d->Head[2] = z;
}

double* stkSurgeryRoute::routeRotation() const
{
	return d_ptr->Rotation;
}

void stkSurgeryRoute::setRouteRotation(double* v)
{
	Q_D(stkSurgeryRoute);
	d->Rotation[0] = v[0];
	d->Rotation[1] = v[1];
	d->Rotation[2] = v[2];
}

void stkSurgeryRoute::setRouteRotation(double x, double y, double z)
{
	Q_D(stkSurgeryRoute);
	d->Rotation[0] = x;
	d->Rotation[1] = y;
	d->Rotation[2] = z;
}

double* stkSurgeryRoute::routeTail() const
{
	return d_ptr->Tail;
}

void stkSurgeryRoute::setRouteTail(double* v)
{
	Q_D(stkSurgeryRoute);
	d->Tail[0] = v[0];
	d->Tail[1] = v[1];
	d->Tail[2] = v[2];
}

void stkSurgeryRoute::setRouteTail(double x, double y, double z)
{
	Q_D(stkSurgeryRoute);
	d->Tail[0] = x;
	d->Tail[1] = y;
	d->Tail[2] = z;
}

int stkSurgeryRoute::routeID() const
{
	return d_ptr->RouteID;
}

void stkSurgeryRoute::setRouteID(int v)
{
	Q_D(stkSurgeryRoute);
	d->RouteID = v;
}