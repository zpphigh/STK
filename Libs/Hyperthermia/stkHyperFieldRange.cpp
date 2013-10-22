#include "stkHyperFieldRange.h"
#include "stkSurgeryRoute.h"
#include "stkThermalField.h"

class stkHyperFieldRangePrivate
{
public:
	double Origin[3];
	double Spacing[3];
	int  Size[3];
	int FieldSize;

	QList<stkSurgeryRoute*> Routes;
	stkThermalField* pInjuryField3D;
	stkThermalField* pTemperatureField3D;
};

stkHyperFieldRange::stkHyperFieldRange(QObject *parent)
	: QObject(parent),d_ptr(new stkHyperFieldRangePrivate)
{
	Q_D(stkHyperFieldRange);
	d->pInjuryField3D = NULL;
	d->pTemperatureField3D = NULL;
	this->clearRoutes();
}

stkHyperFieldRange::~stkHyperFieldRange()
{
	Q_D(stkHyperFieldRange);
	SAFE_DELETE_ARRAY(d->pInjuryField3D);
	SAFE_DELETE_ARRAY(d->pTemperatureField3D);
}

double* stkHyperFieldRange::rangeOrigin() const
{
	return d_ptr->Origin;
}

void stkHyperFieldRange::setRangeOrigin(double* v)
{
	Q_D(stkHyperFieldRange);
	d->Origin[0] = v[0];
	d->Origin[1] = v[1];
	d->Origin[2] = v[2];
}

void stkHyperFieldRange::setRangeOrigin(double x, double y, double z)
{
	Q_D(stkHyperFieldRange);
	d->Origin[0] = x;
	d->Origin[1] = y;
	d->Origin[2] = z;
}

double* stkHyperFieldRange::rangeSpacing() const
{
	return d_ptr->Spacing;
}

void stkHyperFieldRange::setRangeSpacing(double* v)
{
	Q_D(stkHyperFieldRange);
	d->Spacing[0] = v[0];
	d->Spacing[1] = v[1];
	d->Spacing[2] = v[2];
}

void stkHyperFieldRange::setRangeSpacing(double x, double y, double z)
{
	Q_D(stkHyperFieldRange);
	d->Spacing[0] = x;
	d->Spacing[1] = y;
	d->Spacing[2] = z;
}

int* stkHyperFieldRange::rangeSize() const
{
	return d_ptr->Size;
}

void stkHyperFieldRange::setRangeSize(int* v)
{
	Q_D(stkHyperFieldRange);
	d->Size[0] = v[0];
	d->Size[1] = v[1];
	d->Size[2] = v[2];
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

void stkHyperFieldRange::setRangeSize(int x, int y, int z)
{
	Q_D(stkHyperFieldRange);
	d->Size[0] = x;
	d->Size[1] = y;
	d->Size[2] = z;
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

QList<stkSurgeryRoute*> stkHyperFieldRange::getRoutes() const
{
	return d_ptr->Routes;
}

stkSurgeryRoute* stkHyperFieldRange::getRoute(int i)
{
	Q_D(stkHyperFieldRange);
	if (i>=d->Routes.size() || i<0)
		return NULL;

	return d->Routes[i];
}

void stkHyperFieldRange::clearRoutes()
{
	Q_D(stkHyperFieldRange);
	d->Routes.clear();
}

void stkHyperFieldRange::addRoute(stkSurgeryRoute* route)
{
	Q_D(stkHyperFieldRange);
	if (!d->Routes.contains(route))
		d->Routes.push_back(route);
}

int stkHyperFieldRange::getNumberOfRoutes() const
{
	return d_ptr->Routes.size();
}

stkThermalField* stkHyperFieldRange::getThermalField3D()
{
	Q_D(stkHyperFieldRange);
	if (!d->pTemperatureField3D)
	{
		d->pTemperatureField3D = new stkThermalField;
	}
	return d->pTemperatureField3D;
}

stkThermalField* stkHyperFieldRange::getInjuryField3D()
{
	Q_D(stkHyperFieldRange);
	if (!d->pInjuryField3D)
	{
		d->pInjuryField3D = new stkThermalField;
	}
	return d->pInjuryField3D;
}