#include "stkMicrowaveAblation.h"
#include "stkMicrowaveRoute.h"
#include "stkThermalField.h"

class stkMicrowaveAblationPrivate
{
public:
	void clear();
	bool computeSingleRange(int i, HeatFieldRange &SingleRange);
	bool oneNeedleEnergy(int* size, double* spacing, double* origin, int TimeInterval,double paramPower, double paramAxis, double AttenuationFactor,double power, double* tail,double* tip, float* OneNeedleEnergyField);
	bool computeTemperature(int* size, double* spacing, int TimeInterval, float* pEnergyField, float* pInjuryField, float* InjuryFieldOld, float* TempratureField, float* TempratureFieldOld);


	QList<stkMicrowaveRoute*> MicrowaveRoutes;
	int TotalTime;
	int FieldTime;
	double Origin[3];
	double Spacing[3];
	int Size[3];
	int MaxPoint[3];
	int TimeInterval;
	int TimeSteps;
	int FieldSize;

	float *EnergyField;
	float* ThermalField;
	float* InjuryField;

	QList<HeatFieldRange> Ranges;
};

void stkMicrowaveAblationPrivate::clear()
{
	MicrowaveRoutes.clear();
	FieldTime = -1;
	TotalTime = 1;
	TimeInterval = 1;
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

bool stkMicrowaveAblationPrivate::computeSingleRange(int i, HeatFieldRange &SingleRange)
{
	double Xmax,Xmin,Ymax,Ymin,Zmax,Zmin;

	double* point =  MicrowaveRoutes[i]->routeHead();
	
	Xmax = point[0];
	Xmin = point[0];
	Ymax = point[1];
	Ymin = point[1];
	Zmax = point[2];
	Zmin = point[2];

	point =  MicrowaveRoutes[i]->routeTail(); 

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
	SingleRange.Origin[0] =Xmin;
	SingleRange.Origin[1] =Ymin;   //原点位置可正可负
	SingleRange.Origin[2] =Zmin;

	double maxPoint[3];
	maxPoint[0] = Xmax;
	maxPoint[1] = Ymax;   
	maxPoint[2] = Zmax;

	/////////////////////////////////////////////5.18///////////////////////////////////////////
	/////////////////////////////////////////////5.18///////////////////////////////////////////
	for(int i = 0; i < 3; i++)
		SingleRange.Size[i] =(int) (( maxPoint[i]-SingleRange.Origin[i])/double(int(Spacing[i]+0.5)))+1;

	// 设置空间步长, 再反过来对spacing做微小修正，提高结果精度
	for(int i = 0; i < 3; i++)
	{
		Spacing[i]= (maxPoint[i] - SingleRange.Origin[i])/(SingleRange.Size[i]-1);
		SingleRange.Spacing[i] = Spacing[i];
	}

	TimeSteps = TotalTime/TimeInterval+1;

	return true;
}

bool stkMicrowaveAblationPrivate::oneNeedleEnergy(int* size, double* spacing, double* origin, int timeInterval,double paramPower, double paramAxis, double pAttenuationFactor,double power, double* tail, double* tip, float* OneNeedleEnergyField )
{   
	double PP1,PP2,P1P2,PMidle;            
	double Perimeter,Rneedle,Zneedle;
	int i,j,k;

	int Size0xSize1 = size[0]*size[1];
	int Size0xSize1xSize2 = size[0]*size[1]*size[2];
	//	int FieldSize = Size0xSize1xSize2 * sizeof(double);

	double Ct,ct=paramPower;
	double z0=paramAxis;

	//	mtkVolumePointType p,Midlepoint;
	double p[3];;
	double Midlepoint[3];

	Midlepoint[0]=(tip[0]+tail[0])/2.0;
	Midlepoint[1]=(tip[1]+tail[1])/2.0;
	Midlepoint[2]=(tip[2]+tail[2])/2.0;

	//   printf("  %f, %f,%f \n",tail[0],tail[1], tail[2]);
	//printf("  %f, %f,%f \n",tip[0],tip[1], tip[2]);
	//printf("  %f, %f,%f \n",origin[0],origin[1], origin[2]);
	//double zy = pow((double)((-(spacing[1]-2)*(spacing[1]-8))/70+1),(double)1.8);
	//Ct = ct * zy;
	for(i=0;i<size[0];i++)
	{ 
		for(j=0;j<size[1];j++)
		{
			for(k=0;k<size[2];k++)
			{   
				p[0]=origin[0]+i*spacing[0];p[1]=origin[1]+j*spacing[1];p[2]=origin[2]+k*spacing[2];
				if(spacing[1]>=2)
				{if(spacing[1]<=8){Ct=ct*pow((double)((-(spacing[1]-2)*(spacing[1]-8))/70+1),(double)1.8);}}

				//三维坐标转化到以第n根针为轴线的二维柱坐标
				PP1=sqrt((p[0]-tip[0])*(p[0]-tip[0])+(p[1]-tip[1])*(p[1]-tip[1])+(p[2]-tip[2])*(p[2]-tip[2]));
				PP2=sqrt((p[0]-tail[0])*(p[0]-tail[0])+(p[1]-tail[1])*(p[1]-tail[1])+(p[2]-tail[2])*(p[2]-tail[2]));
				P1P2=sqrt((tip[0]-tail[0])*(tip[0]-tail[0])+(tip[1]-tail[1])*(tip[1]-tail[1])+(tip[2]-tail[2])*(tip[2]-tail[2]));
				PMidle=sqrt((p[0]-Midlepoint[0])*(p[0]-Midlepoint[0])+(p[1]-Midlepoint[1])*(p[1]-Midlepoint[1])+(p[2]-Midlepoint[2])*(p[2]-Midlepoint[2]));
				Perimeter=(PP1+PP2+P1P2)/2.0;
				Rneedle=2.0*sqrt(Perimeter*(Perimeter-PP1)*(Perimeter-PP2)*(Perimeter-P1P2))/P1P2;
				Zneedle=sqrt(abs(PMidle*PMidle-Rneedle*Rneedle));     //r和z均以毫米为单位
				//printf("size:   %f,%f\n",Rneedle,Zneedle);
				if(Rneedle<2){Rneedle=2;}
				OneNeedleEnergyField[(k*Size0xSize1+j*size[0]+i)]=Ct*power*(2.0*pAttenuationFactor*Rneedle+(2.2-2))/(pow((double)Rneedle,2.2))*exp(-2.0*pAttenuationFactor*Rneedle)*exp(-Zneedle*Zneedle/z0/z0)*1.0e9;
				//printf("OneEnergy:   %f\n",OneNeedleEnergyField[(k*Size0xSize1+j*size[0]+i)]);
				if(OneNeedleEnergyField[(k*Size0xSize1+j*size[0]+i)]<0.0)                  
				{OneNeedleEnergyField[(k*Size0xSize1+j*size[0]+i)]=0;}
			}
		}
	}
	return 0;
}

stkMicrowaveAblation::stkMicrowaveAblation(QObject *parent)
	: QObject(parent),d_ptr(new stkMicrowaveAblationPrivate)
{
	Q_D(stkMicrowaveAblation);
	d->EnergyField = NULL;
	d->ThermalField = NULL;
	d->InjuryField = NULL;
	d->clear();
}

stkMicrowaveAblation::~stkMicrowaveAblation()
{
	Q_D(stkMicrowaveAblation);
	SAFE_DELETE_ARRAY(d->ThermalField);
	SAFE_DELETE_ARRAY(d->EnergyField);
	SAFE_DELETE_ARRAY(d->InjuryField);
}

double* stkMicrowaveAblation::ablationOrigin() const
{
	return d_ptr->Origin;
}

void stkMicrowaveAblation::setAblationOrigin(double* v)
{
	Q_D(stkMicrowaveAblation);
	d->Origin[0] = v[0];
	d->Origin[1] = v[1];
	d->Origin[2] = v[2];
}

void stkMicrowaveAblation::setAblationOrigin(double x, double y, double z)
{
	Q_D(stkMicrowaveAblation);
	d->Origin[0] = x;
	d->Origin[1] = y;
	d->Origin[2] = z;
}

double* stkMicrowaveAblation::ablationSpacing() const
{
	return d_ptr->Spacing;
}

void stkMicrowaveAblation::setAblationSpacing(double* v)
{
	Q_D(stkMicrowaveAblation);
	d->Spacing[0] = v[0];
	d->Spacing[1] = v[1];
	d->Spacing[2] = v[2];
}

void stkMicrowaveAblation::setAblationSpacing(double x, double y, double z)
{
	Q_D(stkMicrowaveAblation);
	d->Spacing[0] = x;
	d->Spacing[1] = y;
	d->Spacing[2] = z;
}

int* stkMicrowaveAblation::ablationSize() const
{
	return d_ptr->Size;
}

void stkMicrowaveAblation::setAblationSize(int* v)
{
	Q_D(stkMicrowaveAblation);
	d->Size[0] = v[0];
	d->Size[1] = v[1];
	d->Size[2] = v[2];
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

void stkMicrowaveAblation::setAblationSize(int x, int y, int z)
{
	Q_D(stkMicrowaveAblation);
	d->Size[0] = x;
	d->Size[1] = y;
	d->Size[2] = z;
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

int stkMicrowaveAblation::totalTime() const
{
	return d_ptr->TotalTime;
}

void stkMicrowaveAblation::setTotalTime(int v)
{
	Q_D(stkMicrowaveAblation);
	d->TotalTime = v;
}

int stkMicrowaveAblation::timeInterval() const
{
	return d_ptr->TimeInterval;
}

void stkMicrowaveAblation::setTimeInterval(int v)
{
	Q_D(stkMicrowaveAblation);
	d->TimeInterval = v;
}

int stkMicrowaveAblation::fieldTime() const
{
	return d_ptr->FieldTime;
}

void stkMicrowaveAblation::setFieldTime(int v)
{
	Q_D(stkMicrowaveAblation);
	d->FieldTime = v;
}

int* stkMicrowaveAblation::maxPoint() const
{
	return d_ptr->MaxPoint;
}

void stkMicrowaveAblation::setMaxPoint(int* v)
{
	Q_D(stkMicrowaveAblation);
	d->MaxPoint[0] = v[0];
	d->MaxPoint[1] = v[1];
	d->MaxPoint[2] = v[2];
}

void stkMicrowaveAblation::setMaxPoint(int x, int y, int z)
{
	Q_D(stkMicrowaveAblation);
	d->MaxPoint[0] = x;
	d->MaxPoint[1] = y;
	d->MaxPoint[2] = z;
}

int stkMicrowaveAblation::timeSteps() const
{
	return d_ptr->TimeSteps;
}

float* stkMicrowaveAblation::getThermalField() const
{
	return d_ptr->ThermalField;
}

int stkMicrowaveAblation::getNumberOfNeedle() const
{
	return d_ptr->MicrowaveRoutes.size();
}

int stkMicrowaveAblation::getNumberOfNeedle( int time ,QList<stkMicrowaveRoute*> &routes) const
{
	int count = 0;
	for( int i = 0 ; i < routes.size(); i++ )
	{
		if( time >= routes[i]->startTime() && time <= routes[i]->endTime())
			count++;
	}

	return count;
}

bool stkMicrowaveAblation::computeMultiRange()
{
	Q_D(stkMicrowaveAblation);
	d->Ranges.clear();

	for( int i = 0 ; i < getNumberOfNeedle(); i++ )
	{
		HeatFieldRange* tempRange = new HeatFieldRange;	
		tempRange->Routes.clear();
		d->computeSingleRange(i, *tempRange);
		tempRange->Routes.push_back(d->MicrowaveRoutes[i]);

		d->Ranges.push_back(*tempRange);
	}
	return true;
}

void stkMicrowaveAblation::addMultiHeatRange()
{
	Q_D(stkMicrowaveAblation);
	bool HasIntersectedRegion = false;
	for (QList<HeatFieldRange>::iterator HeadIter = d->Ranges.begin(); HeadIter < d->Ranges.end(); HeadIter++)
	{
		for (QList<HeatFieldRange>::iterator Iter = HeadIter + 1; Iter < d->Ranges.end(); Iter ++)
		{
			//mtkVolumeInfo* tempInfo = NULL;
			//if (HeadIter->ROI.IsIntersect(Iter->ROI))
			//{
			//	tempInfo = new mtkVolumeInfo;
			//	HasIntersectedRegion = true;
			//	*tempInfo = HeadIter->ROI.Add(Iter->ROI);
			//}
			//if (tempInfo != NULL)
			//{
			//	HeadIter->ROI = *tempInfo;
			//	for (int i = 0; i < Iter->Routes.size(); i ++)
			//	{
			//		HeadIter->Routes.push_back(Iter->Routes[i]);
			//	}

			//	d->Ranges.erase(Iter);
			//	break;
			//}
		}
		if (HasIntersectedRegion == true)
		{
			break;
		}
	}
	if (HasIntersectedRegion)
	{
		addMultiHeatRange();
	}else
	{
		return;
	}
}

QList<HeatFieldRange> stkMicrowaveAblation::getRanges() const
{
	return d_ptr->Ranges;
}

bool stkMicrowaveAblation::allocateMemory(int index)
{
	Q_D(stkMicrowaveAblation);
	d->TimeSteps = (int) ( d->TotalTime/ d->TimeInterval);    //时间以秒为单位
	d->FieldSize = d->Size[0]*d->Size[1]*d->Size[2];

	d->EnergyField = new float[d->FieldSize];
	ZeroMemory(d->EnergyField, sizeof(float) * d->FieldSize );

	//////////////////////////////////////////////////////////////////////////////
	////根据各种信息分配buffer
	d->Ranges[index].pTemperatureField3D = new stkThermalField(NULL);
	d->Ranges[index].pInjuryField3D = new stkThermalField(NULL);

	d->Ranges[index].pTemperatureField3D->setFieldSpacing(d->Spacing);
	d->Ranges[index].pTemperatureField3D->setFieldOrigin(d->Origin);
	d->Ranges[index].pTemperatureField3D->setFieldSize(d->Size);
	d->Ranges[index].pTemperatureField3D->allocateMemory(); //分配buffer

	d->Ranges[index].pInjuryField3D->setFieldSpacing(d->Spacing);
	d->Ranges[index].pInjuryField3D->setFieldOrigin(d->Origin);
	d->Ranges[index].pInjuryField3D->setFieldSize(d->Size);
	d->Ranges[index].pInjuryField3D->allocateMemory(); //分配buffer
	////////////////////////////////////////////////////////////////////////////

	d->ThermalField = new float[d->FieldSize];
	d->InjuryField = new float[d->FieldSize];

	ZeroMemory(d->ThermalField, sizeof(float) * d->FieldSize  ); 
	ZeroMemory(d->InjuryField, sizeof(float) * d->FieldSize );

	return true;
}

void stkMicrowaveAblation::addNeedle(stkMicrowaveRoute* pNeedle)
{
	Q_D(stkMicrowaveAblation);
	d->MicrowaveRoutes.push_back(pNeedle);
}

stkMicrowaveRoute* stkMicrowaveAblation::getNeedle( int i)
{
	Q_D(stkMicrowaveAblation);
	if( i >= d->MicrowaveRoutes.size() || i < 0 )
		return NULL;

	return d->MicrowaveRoutes[i];
}

stkMicrowaveRoute* stkMicrowaveAblation::getNeedle( int time, int index, QList<stkMicrowaveRoute*> &routes)
{
	Q_D(stkMicrowaveAblation);
	if( index < 0 || index >= getNumberOfNeedle(time,routes) )
		return NULL;

	int count = -1;
	for( int i = 0 ; i < routes.size(); i++ )
	{
		if( time >= routes[i]->startTime() && time <= routes[i]->endTime())
		{
			count++;
			if( count == index )
				return routes[i];
		}
	}

	return NULL;
}

bool stkMicrowaveAblation::updateMultiFields(int index)
{
	Q_D(stkMicrowaveAblation);
	//FitEnergyField* fitenrgyfield = new FitEnergyField(NULL);
	//FitTemperatureField* fittempfield = new FitTemperatureField(NULL);
	//FitInjuryField* fitinjuryfield = new FitInjuryField(NULL);
	//TODO
	QList<stkMicrowaveRoute*> RoutesInAField = d->Ranges[index].Routes;
	int n,l,i,j,k;                  //用到的中间变量
	//	double ct= 0.0219339526876446;//0.012           //参数可变,调整热损伤大小
	double AttenuationFactor=0.04;
	float* pTemperatureFieldSizeArray = d->Ranges[index].pTemperatureField3D->fieldBuffer();
	float* pInjuryFieldSizeArray = d->Ranges[index].pInjuryField3D->fieldBuffer();


	float lamdax,lamday,lamdaz;           


	float *OneNeedleEnergyField;
	int Size0xSize1 = d->Size[0]*d->Size[1];
	int Size0xSize1xSize2 = d->FieldSize;
//	int FieldSize = Size0xSize1xSize2 * sizeof(float);
	float dxdydz=d->Spacing[0]*d->Spacing[1]*d->Spacing[2];  //单元格体积，单位：立方毫米


	//*************************** 计算组织温度场 ***************************
	lamdax=d->TimeInterval/(d->Spacing[0]/1000) /(d->Spacing[0]/1000);     
	lamday=d->TimeInterval/(d->Spacing[1]/1000)/(d->Spacing[1]/1000); //求解温度场时，所有的变量的单位都换算成标准单位
	lamdaz=d->TimeInterval/(d->Spacing[2]/1000)/(d->Spacing[2]/1000);

	// ************** 温度场边界条件和初始条件	*******************
	//double TemperatureOfTissue = stkHyperTissueSetting::getTissueInitialTemperature();
	double TemperatureOfTissue = 37;

	l=0;
	for(i=0;i<=(d->Size[0]-1);i++)
	{
		for(j=0;j<=(d->Size[1]-1);j++)
		{ 
			for(k=0;k<=(d->Size[2]-1);k++)
			{
				pTemperatureFieldSizeArray[(k*Size0xSize1+j*d->Size[0]+i)]=TemperatureOfTissue;          //初始条件
				pInjuryFieldSizeArray[(k*Size0xSize1+j*d->Size[0]+i)]=0;
			}
		}	
	}	


	bool bCalcEnergyField;

	/////////////////////////////////////////////////////////////////////////////////
//	d->TimeStep = 5;//////////////////////////////////////////////////////////////////
//	d->TimeInterval = 2;//////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	for(l=1;l<d->TimeSteps;l++)
	{ 
		//***************** 计算微波能量分布 ******************************
		int time = d->TimeInterval*l;


		//损伤场	

		d->InjuryField = pInjuryFieldSizeArray;

		d->ThermalField = pTemperatureFieldSizeArray;

		bCalcEnergyField = false;
		int timepoint;
		for( n = 0 ; n < RoutesInAField.size(); n++ )
		{
			timepoint = RoutesInAField[n]->startTime();
			if( time >= timepoint && time - d->TimeInterval <= timepoint )
			{
				bCalcEnergyField = true;
				break;
			}

			timepoint = RoutesInAField[n]->endTime();
			if( time >= timepoint && time - d->TimeInterval <= timepoint )
			{
				bCalcEnergyField = true;
				break;
			}
		}

		if( bCalcEnergyField )
		{
			ZeroMemory(d->EnergyField, Size0xSize1xSize2 * sizeof(float));                  //能量场设为0
//			d->pEnergyField = fitenrgyfield->Update(d->pEnergyField, time, RoutesInAField,d->Size,d->Spacing,d->Origin,d->TimeInterval, AttenuationFactor, OneNeedleEnergyField);
			//fitenrgyfield->Update(d->pEnergyField, time, RoutesInAField,d->Size,d->Spacing,d->Origin,d->TimeInterval, AttenuationFactor, OneNeedleEnergyField);
			//d->pEnergyField = fitenrgyfield->get();

			for(n=0;n<getNumberOfNeedle(time, RoutesInAField);n++)
			/////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////
			{
//				fitenrgyfield->Update(RoutesInAField,d->Size,d->Spacing,d->Origin,d->TimeInterval, AttenuationFactor, OneNeedleEnergyField);
				stkMicrowaveRoute* pNeedle = this->getNeedle(time,n, RoutesInAField);
				if( pNeedle == NULL)
					continue;

				//D3DXVECTOR3 NeedleTip = *(pNeedle->GetPosition()); 
				//D3DXVECTOR3 NeedTail =  *(pNeedle->GetTail());
				double* NeedleTip = pNeedle->routeHead(); 
				double* NeedTail = pNeedle->routeTail();

				double tip[3];
				double tail[3];
				tip[0]=NeedleTip[0]; tip[1]=NeedleTip[1];tip[2]=NeedleTip[2];
				tail[0]=NeedTail[0]; tail[1]=NeedTail[1];tail[2]=NeedTail[2];
				double w = pNeedle->power();
				double Ct = pNeedle->ct();
				double z0 = pNeedle->z0();

				if (Ct == 0.00 && z0 == 0.00)
				{
					//pNeedle->GetCtZ0();
					//Ct = pNeedle->GetCt();
					//z0 = pNeedle->GetZ0();
					Ct = 0.0219339526876446;
					z0 = 14.0513591484465;//14.0513591484465" Ct="0.0219339526876446"/>
				}
				/////////////////////////////////////////////////////////////////////////////////
				/////////////////////////////////////////////////////////////////////////////////
				//Ct = 0.0219339526876446;
				//z0 = 14.0513591484465;//14.0513591484465" Ct="0.0219339526876446"/>
				Ct = 0.017;
				z0 = 8.166;//14.0513591484465" Ct="0.0219339526876446"/>
				/////////////////////////////////////////////////////////////////////////////////
				/////////////////////////////////////////////////////////////////////////////////
				pNeedle->setCt(Ct);
				pNeedle->setZ0(z0);

				// Ct,z0 更新
				//printf("***************************************************\n");
				//printf("power: %f\n",w);

				OneNeedleEnergyField=new float[Size0xSize1xSize2];
				d->oneNeedleEnergy(d->Size,d->Spacing,d->Origin,d->TimeInterval,Ct, z0, AttenuationFactor,w,tail,tip, OneNeedleEnergyField);
				//printf("Energy:   %f\n",OneNeedleEnergyField[((int)((d->Size[2]-1)/2)*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))]);


				for(i=0;i<d->Size[0];i++)
				{ 
					for(j=0;j<d->Size[1];j++)
					{
						for(k=0;k<d->Size[2];k++)
						{
							d->EnergyField[(k*Size0xSize1+j*d->Size[0]+i)]=d->EnergyField[(k*Size0xSize1+j*d->Size[0]+i)]+OneNeedleEnergyField[(k*Size0xSize1+j*d->Size[0]+i)];
						}
					}
				}
				SAFE_DELETE_ARRAY(OneNeedleEnergyField);
			}
		}

		//fittempfield->Update(d->Size,d->Spacing,d->TimeInterval,d->pEnergyField,d->pInjuryFieldOld,d->pTemperatureFieldOld);
		//pTemperatureFieldSizeArray = fittempfield->get();
		//fitinjuryfield->Update(fittempfield->getInj());
		//pInjuryFieldSizeArray = fitinjuryfield->get();
		d->computeTemperature(d->Size,d->Spacing,d->TimeInterval,d->EnergyField,pInjuryFieldSizeArray,d->InjuryField,pTemperatureFieldSizeArray,d->ThermalField);
		//printf("Tempeture:  %d,  %f, %f:\n",d->TimeInterval*l,d->pTemperatureField[(2*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))],d->pEnergyField[(0*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))]);

	}

	return true;
}

bool stkMicrowaveAblationPrivate::computeTemperature(int* size,double* spacing,int timeInterval,float *EnergyField,float *InjuryField,float *InjuryFieldOld,float *TemperatureField,float *TemperatureFieldOld)
{ 
	double wb,kk,cc,ft;                             
	float *A,*B,*C,*D,*U,*V,*W;
	int i,j,k;

	int Size0xSize1 = size[0]*size[1];
	int Size0xSize1xSize2 = size[0]*size[1]*size[2];
	//	int FieldSize = Size0xSize1xSize2 * sizeof(double);

	U=new float[Size0xSize1xSize2]; 
	V=new float[Size0xSize1xSize2];
	W=new float[Size0xSize1xSize2];

	//double TemperatureOfTissue = stkHyperTissueSetting::getTissueInitialTemperature();
	double TemperatureOfTissue = 37;

	for(i=0;i<size[0];i++)
	{ 
		for(j=0;j<size[1];j++)
		{
			for(k=0;k<size[2];k++)
			{
				U[(k*Size0xSize1+j*size[0]+i)]=TemperatureOfTissue;
				V[(k*Size0xSize1+j*size[0]+i)]=TemperatureOfTissue;
				W[(k*Size0xSize1+j*size[0]+i)]=TemperatureOfTissue;
			}
		}
	}

	double lamdax=timeInterval/(spacing[0]/1000) /(spacing[0]/1000);     
	double lamday=timeInterval/(spacing[1]/1000)/(spacing[1]/1000); //求解温度场时，所有的变量的单位都换算成标准单位
	double lamdaz=timeInterval/(spacing[2]/1000)/(spacing[2]/1000);

	// x方向1/3隐式
	A=new float[size[0]];
	B=new float[size[0]];
	C=new float[size[0]];
	D=new float[size[0]];

	for(k=1;k<(size[2]-1);k++)
	{
		for(j=1;j<(size[1]-1);j++)
		{    
			i=1;          // 考虑组织物性参数和血液灌注随温度和热损伤程度的变化而变化
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}     // 热损伤部位的血液灌注为零
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else     // 热损伤前，血液灌注率随温度和热损伤程度而变化
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[i]=1.0/2.0*lamdax*kk;
			A[i]=lamdax*kk+m0*cc;
			C[i]=0;
			D[i]=kk*(lamdax*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i+1)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i-1)])+lamday*(TemperatureFieldOld[(k*Size0xSize1+(j+1)*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+(j-1)*size[0]+i)])+lamdaz*(TemperatureFieldOld[((k+1)*Size0xSize1+j*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[((k-1)*Size0xSize1+j*size[0]+i)]))+(EnergyField[(k*Size0xSize1+j*size[0]+i)]-wb*mb*cb*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-Tb))*timeInterval+1.0/2.0*lamdax*kk*37;

			for(i=2;i<(size[0]-2);i++)
			{
				if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
				{wb=0.0;}
				else
				{
					if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
					{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
					else
					{ft=4;}
					wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
				}

				kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
				cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);

				B[i]=1.0/2.0*lamdax*kk;
				A[i]=lamdax*kk+m0*cc;
				C[i]=1.0/2.0*lamdax*kk;
				D[i]=kk*(lamdax*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i+1)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i-1)])+lamday*(TemperatureFieldOld[(k*Size0xSize1+(j+1)*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+(j-1)*size[0]+i)])+lamdaz*(TemperatureFieldOld[((k+1)*Size0xSize1+j*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[((k-1)*Size0xSize1+j*size[0]+i)]))+(EnergyField[(k*Size0xSize1+j*size[0]+i)]-wb*mb*cb*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-Tb))*timeInterval;
			}


			i=(size[0]-2);
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[i]=0;
			A[i]=lamdax*kk+m0*cc;
			C[i]=1.0/2.0*lamdax*kk;
			D[i]=kk*(lamdax*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i+1)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i-1)])+lamday*(TemperatureFieldOld[(k*Size0xSize1+(j+1)*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[(k*Size0xSize1+(j-1)*size[0]+i)])+lamdaz*(TemperatureFieldOld[((k+1)*Size0xSize1+j*size[0]+i)]-2.0*TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+TemperatureFieldOld[((k-1)*Size0xSize1+j*size[0]+i)]))+(EnergyField[(k*Size0xSize1+j*size[0]+i)]-wb*mb*cb*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-Tb))*timeInterval+1.0/2.0*lamdax*kk*37;

			i=1;
			B[i]=B[i]/A[i];
			D[i]=D[i]/A[i];
			for(i=2;i<(size[0]-1);i++)
			{
				B[i]=B[i]/(A[i]-C[i]*B[i-1]);  
				D[i]=(D[i]+C[i]*D[i-1])/(A[i]-C[i]*B[i-1]);    
			}
			U[(k*Size0xSize1+j*size[0]+(size[0]-2))]=D[(size[0]-2)];
			for(i=(size[0]-3);i>=1;i--)
			{U[(k*Size0xSize1+j*size[0]+i)]=B[i]*U[(k*Size0xSize1+j*size[0]+i+1)]+D[i];}
		}	
	}	 


	SAFE_DELETE_ARRAY(A);
	SAFE_DELETE_ARRAY(B);
	SAFE_DELETE_ARRAY(C);
	SAFE_DELETE_ARRAY(D);

	// y方向1/3隐式 

	A=new float[size[1]];
	B=new float[size[1]];
	C=new float[size[1]];
	D=new float[size[1]];

	for(i=1;i<(size[0]-1);i++)
	{
		for(k=1;k<(size[2]-1);k++)
		{  
			j=1;
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[j]=1.0/2.0*lamday*kk;
			A[j]=lamday*kk+m0*cc;
			C[j]=0;
			D[j]=m0*cc*U[(k*Size0xSize1+j*size[0]+i)]+1.0/2.0*lamday*kk*37;

			for(j=2;j<(size[1]-2);j++)
			{
				if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
				{wb=0.0;}
				else
				{
					if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
					{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
					else
					{ft=4;}
					wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
				}

				kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
				cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);

				B[j]=1.0/2.0*lamday*kk;
				A[j]=lamday*kk+m0*cc;
				C[j]=1.0/2.0*lamday*kk;
				D[j]=m0*cc*U[(k*Size0xSize1+j*size[0]+i)];
			}

			j=(size[1]-2);
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[j]=0;
			A[j]=lamday*kk+m0*cc;
			C[j]=1.0/2.0*lamday*kk;
			D[j]=m0*cc*U[(k*Size0xSize1+j*size[0]+i)]+1.0/2.0*lamday*kk*37;

			j=1;
			B[j]=B[j]/A[j];
			D[j]=D[j]/A[j];
			for(j=2;j<(size[1]-1);j++)
			{
				B[j]=B[j]/(A[j]-C[j]*B[j-1]);  
				D[j]=(D[j]+C[j]*D[j-1])/(A[j]-C[j]*B[j-1]);    
			}
			V[(k*Size0xSize1+(size[1]-2)*size[0]+i)]=D[(size[1]-2)];
			for(j=(size[1]-3);j>=1;j--)
			{V[(k*Size0xSize1+j*size[0]+i)]=B[j]*V[(k*Size0xSize1+(j+1)*size[0]+i)]+D[j];}
		}	
	}	

	SAFE_DELETE_ARRAY(A);
	SAFE_DELETE_ARRAY(B);
	SAFE_DELETE_ARRAY(C);
	SAFE_DELETE_ARRAY(D);

	// z方向1/3隐式

	A=new float[size[2]];
	B=new float[size[2]];
	C=new float[size[2]];
	D=new float[size[2]];

	for(i=1;i<(size[0]-1);i++)
	{
		for(j=1;j<(size[1]-1);j++)
		{ 
			k=1;
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[k]=1.0/2.0*lamdaz*kk;
			A[k]=lamdaz*kk+m0*cc;
			C[k]=0;
			D[k]=m0*cc*V[(k*Size0xSize1+j*size[0]+i)]+1.0/2.0*lamdaz*kk*37;	

			for(k=2;k<(size[2]-2);k++)
			{
				kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
				cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
				if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
				{wb=0.0;}
				else
				{
					if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
					{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
					else
					{ft=4;}
					wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
				}

				B[k]=1.0/2.0*lamdaz*kk;
				A[k]=lamdaz*kk+m0*cc;
				C[k]=1.0/2.0*lamdaz*kk;
				D[k]=m0*cc*V[(k*Size0xSize1+j*size[0]+i)];
			}

			k=(size[2]-2);
			kk=0.419*(0.133+1.36*(1+1.78/1000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			cc=4190*(0.37+0.63*(1+1.016/10000*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-20))*0.69);
			if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=1.0)
			{wb=0.0;}
			else
			{
				if(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]<42)
				{ ft=4+0.6*(TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]-42);}
				else
				{ft=4;}
				wb=wb0*ft*exp(-InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]);
			}

			B[k]=0;
			A[k]=lamdaz*kk+m0*cc;
			C[k]=1.0/2.0*lamdaz*kk;
			D[k]=m0*cc*V[(k*Size0xSize1+j*size[0]+i)]+1.0/2.0*lamdaz*kk*37;


			k=1;
			B[k]=B[k]/A[k];
			D[k]=D[k]/A[k];
			for(k=2;k<(size[2]-1);k++)
			{
				B[k]=B[k]/(A[k]-C[k]*B[k-1]);  
				D[k]=(D[k]+C[k]*D[k-1])/(A[k]-C[k]*B[k-1]);    
			}
			W[((size[2]-2)*Size0xSize1+j*size[0]+i)]=D[size[2]-2];
			TemperatureField[((size[2]-2)*Size0xSize1+j*size[0]+i)]=TemperatureFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)]+W[((size[2]-2)*Size0xSize1+j*size[0]+i)];

			// *************** 计算组织的热损伤函数 ********************************** 
			if(InjuryFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)]>=5e2)    //当热损伤过大时，不再累加，以防溢出
			{InjuryField[((size[2]-2)*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)];}
			else
			{InjuryField[((size[2]-2)*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)]+9.4e104*exp(-6.68e5/8.31/(TemperatureField[((size[2]-2)*Size0xSize1+j*size[0]+i)]+273.12))*timeInterval;}

			// 防止热损伤减小 
			if(InjuryField[((size[2]-2)*Size0xSize1+j*size[0]+i)]<InjuryFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)]) 
			{InjuryField[((size[2]-2)*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[((size[2]-2)*Size0xSize1+j*size[0]+i)];}


			for(k=(size[2]-3);k>=1;k--)
			{
				W[(k*Size0xSize1+j*size[0]+i)]=B[k]*W[((k+1)*Size0xSize1+j*size[0]+i)]+D[k];
				TemperatureField[(k*Size0xSize1+j*size[0]+i)]=TemperatureFieldOld[(k*Size0xSize1+j*size[0]+i)]+W[(k*Size0xSize1+j*size[0]+i)];
				if((TemperatureField[(k*Size0xSize1+j*size[0]+i)]>5000) || (TemperatureField[(k*Size0xSize1+j*size[0]+i)]<-5000) )
				{return false;}


				if(InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]>=5e2)
				{InjuryField[(k*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)];}
				else
				{InjuryField[(k*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)]+9.4e104*exp(-6.68e5/8.31/(TemperatureField[(k*Size0xSize1+j*size[0]+i)]+273.12))*timeInterval;}

				if(InjuryField[(k*Size0xSize1+j*size[0]+i)]<InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)])
				{InjuryField[(k*Size0xSize1+j*size[0]+i)]=InjuryFieldOld[(k*Size0xSize1+j*size[0]+i)];}
			}
		}	
	}

	SAFE_DELETE_ARRAY(U);
	SAFE_DELETE_ARRAY(V);
	SAFE_DELETE_ARRAY(W);
	// 
	// 	SAFE_DELETE_ARRAY(A);
	// 	SAFE_DELETE_ARRAY(B);
	// 	SAFE_DELETE_ARRAY(C);
	// 	SAFE_DELETE_ARRAY(D);
	return 0;
}
