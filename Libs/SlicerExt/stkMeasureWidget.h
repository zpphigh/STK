#pragma once
#include "stkSlicerExtExport.h"
#include "vtkAbstractWidget.h"
#include "stkMeasureRepresentation.h"
#include <map>
#include "vtkSmartPointer.h"

using namespace std;

class STK_SLICEREXT_EXPORT vtkMeasureWidget : public vtkAbstractWidget
{
public:
	vtkTypeMacro(vtkMeasureWidget,vtkAbstractWidget);

	vtkMeasureRepresentation *GetRepresentation()
	{
		return (vtkMeasureRepresentation*)this->WidgetRep;
	}

	//ÇÐ»»MeasureWidgetµÄ½»»¥×´Ì¬
	void RemoveCurrentObserver();
	void AddCurrentObserver();


protected:
	vtkMeasureWidget(void);
	~vtkMeasureWidget(void);

	bool m_WidgetEnabled;
};
