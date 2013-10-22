#include "stkHyperthermiaBase_Private.h"
#include "stkSurgeryRoute.h"
#include "stkHyperFieldRange.h"

stkHyperthermiaBasePrivate::stkHyperthermiaBasePrivate()
{
}

stkHyperthermiaBasePrivate::~stkHyperthermiaBasePrivate()
{
}

void stkHyperthermiaBasePrivate::clear()
{
	HyperRoutes.clear();

	FieldTime = -1;
	//origin
	Origin[0] = 0;
	Origin[1] = 0;
	Origin[2] = 0;
	//MaxPoint
	MaxPoint[0] = 0;
	MaxPoint[1] = 0;
	MaxPoint[2] = 0;
	//size
	Size[0] = 0;
	Size[1] = 0;
	Size[2] = 0;
}

bool stkHyperthermiaBasePrivate::computeSingleRange(int i, stkHyperFieldRange* SingleRange)
{
	double Xmax,Xmin,Ymax,Ymin,Zmax,Zmin;

	double* point =  HyperRoutes[i]->routeHead();
	Xmax = point[0];
	Xmin = point[0];
	Ymax = point[1];
	Ymin = point[1];
	Zmax = point[2];
	Zmin = point[2];

	point =  HyperRoutes[i]->routeTail(); 

	if( point[0] > Xmax ) Xmax = point[0];
	if( point[0] < Xmin ) Xmin = point[0];
	if( point[1] > Ymax ) Ymax = point[1];
	if( point[1] < Ymin ) Ymin = point[1];
	if( point[2] > Zmax ) Zmax = point[2];
	if( point[2] < Zmin ) Zmin = point[2];

	Xmax += SatetyBound;
	Xmin -= SatetyBound;
	Ymax += SatetyBound;
	Ymin -= SatetyBound;
	Zmax += SatetyBound;
	Zmin -= SatetyBound;

	//设置原点
	SingleRange->setRangeOrigin(Xmin,Ymin,Zmin);

	double maxPoint[3];
	maxPoint[0] = Xmax;
	maxPoint[1] = Ymax;   
	maxPoint[2] = Zmax;

	/////////////////////////////////////////////5.18///////////////////////////////////////////
	/////////////////////////////////////////////5.18///////////////////////////////////////////
	int pSize[3];
	for(int i = 0; i < 3; i++)
		pSize[i] =(int) (( maxPoint[i]-SingleRange->rangeOrigin()[i])/double(int(Spacing[i]+0.5)))+1;
	SingleRange->setRangeSize(pSize);


	// 设置空间步长, 再反过来对spacing做微小修正，提高结果精度
	for(int i = 0; i < 3; i++)
	{
		Spacing[i]= (maxPoint[i] - SingleRange->rangeOrigin()[i])/(SingleRange->rangeSize()[i]-1);
	}
	SingleRange->setRangeSpacing(Spacing);

	TimeSteps = TotalTime/TimeInterval+1;

	return true;
}