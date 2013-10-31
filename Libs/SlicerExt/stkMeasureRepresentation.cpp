#include "stkMeasureRepresentation.h"
#include "vtkPlane.h"

vtkMeasureRepresentation::vtkMeasureRepresentation()
{
}


vtkMeasureRepresentation::~vtkMeasureRepresentation()
{
}

int vtkMeasureRepresentation::DistanceToPlane(vtkSmartPointer<vtkPlane> plane)
{
	if (m_ControlPoints.size() == 0)
	{		
		m_DrawWidget = 0;
		return 0;
	}

	std::vector<double*>::iterator it;
	for (it = m_ControlPoints.begin(); it!=m_ControlPoints.end(); it++)
	{
		if( plane->DistanceToPlane((*it)) > 1 ) //任何一点的距离过大就即刻返回0，不需要考虑剩下的点
		{
			m_DrawWidget = 0;
			return 0;
		}
	}

	//所有点距都很小的时候，则返回1
	m_DrawWidget = 1;
	return 1;		
}

void vtkMeasureRepresentation::SetSliceDirection(int i)
{
	this->m_SliceDirection = i;
}

void vtkMeasureRepresentation::SetSliceNormal(double *n)
{
	this->m_Normal[0] = n[0];
	this->m_Normal[1] = n[1];
	this->m_Normal[2] = n[2];
}

double* vtkMeasureRepresentation::GetTextDisplayPosition(double x[3])
{
	x[0] = x[0] + 10*m_Normal[0];
	x[1] = x[1] + 10*m_Normal[1];
	x[2] = x[2] + 10*m_Normal[2];
	return x;
}