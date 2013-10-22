#include "stkThermalField.h"

#define SAFE_DELETE_ARRAY(p) { if(p) {delete[] (p); (p) = NULL;}}
class stkThermalFieldPrivate
{
public:
	double Origin[3];
	double Spacing[3];
	int Size[3];
	float* Buffer;

	int TotalTime;
	int FieldTime;
	int MaxPoint[3];
	int TimeInterval;
	int TimeSteps;
	int FieldSize;
};

stkThermalField::stkThermalField(QObject *parent)
: QObject(parent),d_ptr(new stkThermalFieldPrivate)
{
	Q_D(stkThermalField);
	d->Buffer = NULL;
	this->clear();
}

stkThermalField::~stkThermalField()
{
	Q_D(stkThermalField);
	SAFE_DELETE_ARRAY(d->Buffer);
}

int stkThermalField::timeSteps() const
{
	return d_ptr->TimeSteps;
}

double* stkThermalField::fieldOrigin() const
{
	return d_ptr->Origin;
}

void stkThermalField::setFieldOrigin(double* v)
{
	Q_D(stkThermalField);
	d->Origin[0] = v[0];
	d->Origin[1] = v[1];
	d->Origin[2] = v[2];
}

void stkThermalField::setFieldOrigin(double x, double y, double z)
{
	Q_D(stkThermalField);
	d->Origin[0] = x;
	d->Origin[1] = y;
	d->Origin[2] = z;
}

double* stkThermalField::fieldSpacing() const
{
	return d_ptr->Spacing;
}

void stkThermalField::setFieldSpacing(double* v)
{
	Q_D(stkThermalField);
	d->Spacing[0] = v[0];
	d->Spacing[1] = v[1];
	d->Spacing[2] = v[2];
}

void stkThermalField::setFieldSpacing(double x, double y, double z)
{
	Q_D(stkThermalField);
	d->Spacing[0] = x;
	d->Spacing[1] = y;
	d->Spacing[2] = z;
}

int* stkThermalField::fieldSize() const
{
	return d_ptr->Size;
}

void stkThermalField::setFieldSize(int* v)
{
	Q_D(stkThermalField);
	d->Size[0] = v[0];
	d->Size[1] = v[1];
	d->Size[2] = v[2];
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

void stkThermalField::setFieldSize(int x, int y, int z)
{
	Q_D(stkThermalField);
	d->Size[0] = x;
	d->Size[1] = y;
	d->Size[2] = z;
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

float* stkThermalField::fieldBuffer() const
{
	return d_ptr->Buffer;
}

int stkThermalField::totalTime() const
{
	return d_ptr->TotalTime;
}

void stkThermalField::setTotalTime(int v)
{
	Q_D(stkThermalField);
	d->TotalTime = v;
}

int stkThermalField::timeInterval() const
{
	return d_ptr->TimeInterval;
}

void stkThermalField::setTimeInterval(int v)
{
	Q_D(stkThermalField);
	d->TimeInterval = v;
}

int stkThermalField::fieldTime() const
{
	return d_ptr->FieldTime;
}

void stkThermalField::setFieldTime(int v)
{
	Q_D(stkThermalField);
	d->FieldTime = v;
}

int* stkThermalField::maxPoint() const
{
	return d_ptr->MaxPoint;
}

void stkThermalField::setMaxPoint(int* v)
{
	Q_D(stkThermalField);
	d->MaxPoint[0] = v[0];
	d->MaxPoint[1] = v[1];
	d->MaxPoint[2] = v[2];
}

void stkThermalField::setMaxPoint(int x, int y, int z)
{
	Q_D(stkThermalField);
	d->MaxPoint[0] = x;
	d->MaxPoint[1] = y;
	d->MaxPoint[2] = z;
}

void stkThermalField::allocateMemory()
{
	Q_D(stkThermalField);
	d->TimeSteps = (int) ( d->TotalTime/ d->TimeInterval);    //时间以秒为单位
	if( d->Buffer != NULL ){
		delete d->Buffer;
	}
	d->Buffer = new float [d->FieldSize];
}

void stkThermalField::clear()
{
	Q_D(stkThermalField);
	d->FieldTime = -1;
	d->TotalTime = 1;
	d->TimeInterval = 1;
	//origin
	d->Origin[0] = 0;
	d->Origin[1] = 0;
	d->Origin[2] = 0;
	//MaxPoint
	d->MaxPoint[0] = 0;
	d->MaxPoint[1] = 0;
	d->MaxPoint[2] = 0;
	//size
	d->Size[0] = 0;
	d->Size[1] = 0;
	d->Size[2] = 0;
}

void stkThermalField::update()
{

}