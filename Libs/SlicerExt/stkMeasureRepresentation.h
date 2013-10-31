#pragma once
#include "stkSlicerExtExport.h"
#include "vtkWidgetRepresentation.h"
#include "vtkSmartPointer.h"
#include <vector>

class vtkPlane;
using namespace std;

class STK_SLICEREXT_EXPORT vtkMeasureRepresentation : public vtkWidgetRepresentation
{
public:
	vtkTypeMacro(vtkMeasureRepresentation,vtkWidgetRepresentation);
	virtual int DistanceToPlane(vtkSmartPointer<vtkPlane> plane);

	enum SliceDirection {
		Undefined = 0,
		Axial,
		Coronal,
		Sagittal
	};

	virtual void SetSliceDirection(int i);

	virtual void SetSliceNormal(double *n);

	virtual double* GetTextDisplayPosition(double x[3]);

protected:
	vtkMeasureRepresentation();
	~vtkMeasureRepresentation();

	int m_DrawWidget;
	int m_SliceDirection;
	double m_Normal[3];
	std::vector<double*> m_ControlPoints;
};
