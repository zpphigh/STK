#include "stkMicrowaveHyper.h"
#include "stkHyperthermiaBase_Private.h"
#include "stkThermalField.h"
#include "stkMicrowaveRoute.h"
#include "stkHyperFieldRange.h"
#include "stkHyperAlgorithmFDM.h"

class stkMicrowaveHyperPrivate : public stkHyperthermiaBasePrivate
{
public:
	bool oneNeedleEnergy(int* size, double* spacing, double* origin, int TimeInterval,double paramPower, double paramAxis, double AttenuationFactor,double power, double* tail,double* tip, float* OneNeedleEnergyField);

	float* EnergyField;
	float* ThermalField;
	float* InjuryField;
};

bool stkMicrowaveHyperPrivate::oneNeedleEnergy(int* size, double* spacing, double* origin, int timeInterval,double paramPower, double paramAxis, double pAttenuationFactor,double power, double* tail, double* tip, float* OneNeedleEnergyField )
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

stkMicrowaveHyper::stkMicrowaveHyper(QWidget *parent)
	: stkHyperthermiaBase(parent), d_ptr(new stkMicrowaveHyperPrivate)
{
	Q_D(stkMicrowaveHyper);
	d->EnergyField = NULL;
	d->ThermalField = NULL;
	d->InjuryField = NULL;
	d->clear();
}

stkMicrowaveHyper::~stkMicrowaveHyper()
{
	Q_D(stkMicrowaveHyper);
	SAFE_DELETE_ARRAY(d->ThermalField);
	SAFE_DELETE_ARRAY(d->EnergyField);
	SAFE_DELETE_ARRAY(d->InjuryField);
}

float* stkMicrowaveHyper::getThermalField() const
{
	return d_ptr->ThermalField;
}

bool stkMicrowaveHyper::allocateMemory(int index)
{
	Q_D(stkMicrowaveHyper);
	
	this->setTimeSteps((int) ( this->totalTime()/ this->timeInterval()));    //时间以秒为单位

	int* pSize = this->hyperSize();
	 this->setFieldSize(pSize[0]*pSize[1]*pSize[2]);

	d->EnergyField = new float[this->fieldSize()];
	ZeroMemory(d->EnergyField, sizeof(float) * this->fieldSize() );

	//// 	d->pTemperatureField = new float[d->FieldSize];
	//// 	d->pInjuryField = new float[d->FieldSize];  //热损伤场也是时间的函数
	//////////////////////////////////////////////////////////////////////////////
	////根据各种信息分配buffer
	double start[3];
	start[0] = 0;
	start[1] = 0;
	start[2] = 0;
	//mtkField3DType::IndexType start;
	//start.Fill( 0 );
	//mtkField3DType::RegionType region;
	//region.SetIndex( start );
	//region.SetSize( d->Size );

	QList<stkHyperFieldRange*> pRanges = this->getRanges();
	stkThermalField* pTemperatureField3D = pRanges[index]->getThermalField3D();
	stkThermalField* pInjuryField3D = pRanges[index]->getInjuryField3D();

	pTemperatureField3D->setFieldSpacing(this->hyperSpacing());
	pTemperatureField3D->setFieldOrigin(this->hyperOrigin());
	pTemperatureField3D->setFieldSize(this->hyperSize());
	pTemperatureField3D->allocateMemory(); //分配buffer

	pInjuryField3D->setFieldSpacing(this->hyperSpacing());
	pInjuryField3D->setFieldOrigin(this->hyperOrigin());
	pInjuryField3D->setFieldSize(this->hyperSize());
	pInjuryField3D->allocateMemory(); //分配buffer
	////////////////////////////////////////////////////////////////////////////

	d->ThermalField = new float[this->fieldSize()];
	d->InjuryField = new float[this->fieldSize()];

	//// 	ZeroMemory(d->pTemperatureField, sizeof(float) * d->FieldSize  ); 
	//// 	ZeroMemory(d->pInjuryField, sizeof(float) * d->FieldSize );
	ZeroMemory(d->ThermalField, sizeof(float) * this->fieldSize()  ); 
	ZeroMemory(d->InjuryField, sizeof(float) * this->fieldSize() );

	return true;
}

bool stkMicrowaveHyper::updateMultiFields(int index)
{
	Q_D(stkMicrowaveHyper);
	//FitEnergyField* fitenrgyfield = new FitEnergyField(NULL);
	//FitTemperatureField* fittempfield = new FitTemperatureField(NULL);
	//FitInjuryField* fitinjuryfield = new FitInjuryField(NULL);
	//TODO
	QList<stkHyperFieldRange*> pRanges = this->getRanges();
	QList<stkRouteBase*> RoutesInAField = pRanges[index]->getRoutes();
	int n,l,i,j,k;                  //用到的中间变量
	//	double ct= 0.0219339526876446;//0.012           //参数可变,调整热损伤大小
	double AttenuationFactor=0.04;
	float* pTemperatureFieldSizeArray = pRanges[index]->getThermalField3D()->fieldBuffer();
	float* pInjuryFieldSizeArray = pRanges[index]->getInjuryField3D()->fieldBuffer();


	float lamdax,lamday,lamdaz;           

	float *OneNeedleEnergyField;

	int *pSize = this->hyperSize();
	int Size0xSize1 = pSize[0]*pSize[1];
	int Size0xSize1xSize2 = this->fieldSize();

//	int FieldSize = Size0xSize1xSize2 * sizeof(float);
	double* pSpacing = this->hyperSpacing();
	float dxdydz=pSpacing[0]*pSpacing[1]*pSpacing[2];  //单元格体积，单位：立方毫米


	//*************************** 计算组织温度场 ***************************
	lamdax=this->timeInterval()/(pSpacing[0]/1000) /(pSpacing[0]/1000);     
	lamday=this->timeInterval()/(pSpacing[1]/1000)/(pSpacing[1]/1000); //求解温度场时，所有的变量的单位都换算成标准单位
	lamdaz=this->timeInterval()/(pSpacing[2]/1000)/(pSpacing[2]/1000);

	// ************** 温度场边界条件和初始条件	*******************
	double TemperatureOfTissue = stkHyperTissueSetting::getTissueInitialTemperature();
	l=0;
	for(i=0;i<=(pSize[0]-1);i++)
	{
		for(j=0;j<=(pSize[1]-1);j++)
		{ 
			for(k=0;k<=(pSize[2]-1);k++)
			{
				pTemperatureFieldSizeArray[(k*Size0xSize1+j*pSize[0]+i)]=TemperatureOfTissue;          //初始条件
				pInjuryFieldSizeArray[(k*Size0xSize1+j*pSize[0]+i)]=0;
			}
		}	
	}	

	this->setComputeProgress(0);

	bool bCalcEnergyField;

	/////////////////////////////////////////////////////////////////////////////////
//	d->TimeStep = 5;//////////////////////////////////////////////////////////////////
//	d->TimeInterval = 2;//////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	for(l=1;l<this->timeSteps();l++)
	{ 
		//***************** 计算微波能量分布 ******************************
		int time = this->timeInterval()*l;


		//损伤场	

		d->InjuryField = pInjuryFieldSizeArray;

		d->ThermalField = pTemperatureFieldSizeArray;

		bCalcEnergyField = false;
		int timepoint;
		for( n = 0 ; n < RoutesInAField.size(); n++ )
		{
			timepoint = RoutesInAField[n]->startTime();
			if( time >= timepoint && time - this->timeInterval() <= timepoint )
			{
				bCalcEnergyField = true;
				break;
			}

			timepoint = RoutesInAField[n]->endTime();
			if( time >= timepoint && time - this->timeInterval() <= timepoint )
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

			for(n=0;n<getNumberOfRoutes(time, RoutesInAField);n++)
			/////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////
			{
//				fitenrgyfield->Update(RoutesInAField,d->Size,d->Spacing,d->Origin,d->TimeInterval, AttenuationFactor, OneNeedleEnergyField);
				stkMicrowaveRoute* pNeedle = dynamic_cast<stkMicrowaveRoute*>(this->getRoute(time,n, RoutesInAField));
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
				d->oneNeedleEnergy(this->hyperSize(),this->hyperSpacing(),this->hyperOrigin(),this->timeInterval(),Ct, z0, AttenuationFactor,w,tail,tip, OneNeedleEnergyField);
				//printf("Energy:   %f\n",OneNeedleEnergyField[((int)((d->Size[2]-1)/2)*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))]);


				for(i=0;i<pSize[0];i++)
				{ 
					for(j=0;j<pSize[1];j++)
					{
						for(k=0;k<pSize[2];k++)
						{
							d->EnergyField[(k*Size0xSize1+j*pSize[0]+i)]=d->EnergyField[(k*Size0xSize1+j*pSize[0]+i)]+OneNeedleEnergyField[(k*Size0xSize1+j*pSize[0]+i)];
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
		stkHyperAlgorithmFDM::updateHyperFields(pSize,pSpacing,this->timeInterval(),d->EnergyField,pInjuryFieldSizeArray,d->InjuryField,pTemperatureFieldSizeArray,d->ThermalField);
		//printf("Tempeture:  %d,  %f, %f:\n",d->TimeInterval*l,d->pTemperatureField[(2*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))],d->pEnergyField[(0*Size0xSize1+(int)((d->Size[1]-1)/2)*d->Size[0]+(int)((d->Size[0]-1)/2))]);
		this->setComputeProgress(l*100/(this->timeSteps()-1));
	}
	return true;
}