#include "stkMeasureWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkSmartPointer.h"
#include <map>

using namespace std;

typedef std::map<unsigned long, vtkSmartPointer<vtkMeasureWidget>> MeasureWidgetContainer;
typedef MeasureWidgetContainer::iterator MeasureWidgetIterator;

vtkMeasureWidget::vtkMeasureWidget(void)
{
	m_WidgetEnabled = false;
}


vtkMeasureWidget::~vtkMeasureWidget(void)
{
}

void vtkMeasureWidget::RemoveCurrentObserver()
{
	if (!m_WidgetEnabled)
	{
		return;
	} 
	else
	{
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
		m_WidgetEnabled = false;
	}
}
void vtkMeasureWidget::AddCurrentObserver()
{
	if (m_WidgetEnabled)
	{
		return;
	}
	else
	{
		this->EventTranslator->AddEventsToInteractor(this->Interactor,this->EventCallbackCommand,this->Priority);
		m_WidgetEnabled = true;
	}

}
