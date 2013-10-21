#include "stkMicrowaveRoute.h"

class stkMicrowaveRoutePrivate
{
public:
	double Ct;
	double Z0;
};

stkMicrowaveRoute::stkMicrowaveRoute(QObject *parent)
	: stkSurgeryRoute(parent),d_ptr(new stkMicrowaveRoutePrivate)
{

}

stkMicrowaveRoute::~stkMicrowaveRoute()
{

}

double stkMicrowaveRoute::ct() const
{
	return d_ptr->Ct;
}

void stkMicrowaveRoute::setCt(double v)
{
	Q_D(stkMicrowaveRoute);
	d->Ct = v;
}

double stkMicrowaveRoute::z0() const
{
	return d_ptr->Z0;
}

void stkMicrowaveRoute::setZ0(double v)
{
	Q_D(stkMicrowaveRoute);
	d->Z0 = v;
}