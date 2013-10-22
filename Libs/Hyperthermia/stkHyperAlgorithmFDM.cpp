#include "stkHyperAlgorithmFDM.h"
#include "math.h"

class stkHyperAlgorithmFDMPrivate
{
public:

};

stkHyperAlgorithmFDM::stkHyperAlgorithmFDM(QObject *parent)
	: QObject(parent),d_ptr(new stkHyperAlgorithmFDMPrivate)
{

}

stkHyperAlgorithmFDM::~stkHyperAlgorithmFDM()
{

}

bool stkHyperAlgorithmFDM::
	updateHyperFields(int* size,double* spacing,
	int timeInterval,float *EnergyField,
	float *InjuryField,float *InjuryFieldOld,
	float *TemperatureField,float *TemperatureFieldOld)
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

	double TemperatureOfTissue = 37.0;

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