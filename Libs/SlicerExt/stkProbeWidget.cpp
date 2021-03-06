
// AnnotationModule/VTKWidgets includes
#include "stkProbeRepresentation3D.h"
#include "stkProbeWidget.h"

// VTK includes
#include <vtkAbstractWidget.h>
#include <vtkDistanceWidget.h>
#include <vtkObjectFactory.h>
#include <vtkHandleWidget.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindow.h>
#include <vtkPointHandleRepresentation3D.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkProbeWidget);
vtkCxxRevisionMacro (vtkProbeWidget, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkProbeWidget::vtkProbeWidget()
{
	this->WidgetState = vtkProbeWidget::Start;
	this->ManagesCursor = 1;
	this->CurrentHandle = 0;

	// The widgets for moving the end points. They observe this widget (i.e.,
	// this widget is the parent to the handles).
	this->Point1Widget = vtkHandleWidget::New();
	this->Point1Widget->SetPriority(this->Priority-0.01);
	this->Point1Widget->SetParent(this);
	this->Point1Widget->ManagesCursorOff();

	this->Point2Widget = vtkHandleWidget::New();
	this->Point2Widget->SetPriority(this->Priority-0.01);
	this->Point2Widget->SetParent(this);
	this->Point2Widget->ManagesCursorOff();

	this->LineHandle = vtkHandleWidget::New();
	this->LineHandle->SetPriority(this->Priority-0.01);
	this->LineHandle->SetParent(this);
	this->LineHandle->ManagesCursorOff();

	// Define widget events
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkWidgetEvent::Select,
		this, vtkProbeWidget::SelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, vtkProbeWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
		vtkWidgetEvent::Translate,
		this, vtkProbeWidget::TranslateAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent,
		vtkWidgetEvent::EndTranslate,
		this, vtkProbeWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
		vtkWidgetEvent::Scale,
		this, vtkProbeWidget::ScaleAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
		vtkWidgetEvent::EndScale,
		this, vtkProbeWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, vtkProbeWidget::MoveAction);
}

//----------------------------------------------------------------------
vtkProbeWidget::~vtkProbeWidget()
{
	this->Point1Widget->Delete();
	this->Point2Widget->Delete();
	this->LineHandle->Delete();
}

//----------------------------------------------------------------------
void vtkProbeWidget::SetEnabled(int enabling)
{
	// We do this step first because it sets the CurrentRenderer
	this->Superclass::SetEnabled(enabling);

	// We defer enabling the handles until the selection process begins
	if ( enabling )
	{
		// Don't actually turn these on until cursor is near the end points or the line.
		this->CreateDefaultRepresentation();

		this->Point1Widget->SetRepresentation(reinterpret_cast<vtkProbeRepresentation3D*>
			(this->WidgetRep)->GetPoint1Representation());
		this->Point1Widget->SetInteractor(this->Interactor);
		this->Point1Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

		this->Point2Widget->SetRepresentation(reinterpret_cast<vtkProbeRepresentation3D*>
			(this->WidgetRep)->GetPoint2Representation());
		this->Point2Widget->SetInteractor(this->Interactor);
		this->Point2Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

		this->LineHandle->SetRepresentation(reinterpret_cast<vtkProbeRepresentation3D*>
			(this->WidgetRep)->GetLineHandleRepresentation());
		this->LineHandle->SetInteractor(this->Interactor);
		this->LineHandle->GetRepresentation()->SetRenderer(this->CurrentRenderer);
	}
	else
	{
		this->Point1Widget->SetEnabled(0);
		this->Point2Widget->SetEnabled(0);
		this->LineHandle->SetEnabled(0);
	}
}

//----------------------------------------------------------------------
void vtkProbeWidget::TranslateAction(vtkAbstractWidget *w)
{
	vtkProbeWidget *self = reinterpret_cast<vtkProbeWidget*>(w);
	if ( self->WidgetRep->GetInteractionState() == vtkProbeRepresentation3D::Outside )
	{
		return;
	}

	// Get the event position
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// We are definitely selected
	self->WidgetState = vtkProbeWidget::Active;
	self->GrabFocus(self->EventCallbackCommand);
	double e[2];
	e[0] = static_cast<double>(X);
	e[1] = static_cast<double>(Y);
	reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
		StartWidgetInteraction(e);
	self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL); //for the handles
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	self->EventCallbackCommand->SetAbortFlag(1);
}

//----------------------------------------------------------------------
void vtkProbeWidget::SelectAction(vtkAbstractWidget *w)
{
	vtkProbeWidget *self = reinterpret_cast<vtkProbeWidget*>(w);
	if ( self->WidgetRep->GetInteractionState() == vtkProbeRepresentation3D::Outside )
	{
		return;
	}

	// Modify the state, we are selected
	int state = self->WidgetRep->GetInteractionState();
	if ( state == vtkProbeRepresentation3D::OnP1 )
	{
		reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
			SetInteractionState(vtkProbeRepresentation3D::TranslatingP1);
	}
	else if ( state == vtkProbeRepresentation3D::OnP2 )
	{
		reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
			SetInteractionState(vtkProbeRepresentation3D::TranslatingP2);
	}
	else 
	{
		reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
			SetInteractionState(vtkProbeRepresentation3D::OnLine);
	}

	// Get the event position
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// We are definitely selected
	self->WidgetState = vtkProbeWidget::Active;
	self->GrabFocus(self->EventCallbackCommand);
	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);
	reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
		StartWidgetInteraction(eventPos);
	self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL); //for the handles
	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
}

//----------------------------------------------------------------------
void vtkProbeWidget::ScaleAction(vtkAbstractWidget *w)
{
	vtkProbeWidget *self = reinterpret_cast<vtkProbeWidget*>(w);
	if ( self->WidgetRep->GetInteractionState() == vtkProbeRepresentation3D::Outside )
	{
		return;
	}

	reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
		SetInteractionState(vtkProbeRepresentation3D::Scaling);
	self->Interactor->Disable();
	self->LineHandle->SetEnabled(0);
	self->Interactor->Enable();

	// Get the event position
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// We are definitely selected
	self->WidgetState = vtkProbeWidget::Active;
	self->GrabFocus(self->EventCallbackCommand);
	double eventPos[2];
	eventPos[0] = static_cast<double>(X);
	eventPos[1] = static_cast<double>(Y);
	reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
		StartWidgetInteraction(eventPos);
	self->EventCallbackCommand->SetAbortFlag(1);
	self->StartInteraction();
	self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
}

//----------------------------------------------------------------------
void vtkProbeWidget::MoveAction(vtkAbstractWidget *w)
{
	vtkProbeWidget *self = reinterpret_cast<vtkProbeWidget*>(w);
	// compute some info we need for all cases
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// See whether we're active
	if ( self->WidgetState == vtkProbeWidget::Start )
	{
		self->Interactor->Disable(); //avoid extra renders
		self->Point1Widget->SetEnabled(0);
		self->Point2Widget->SetEnabled(0);
		self->LineHandle->SetEnabled(0);

		int oldState = self->WidgetRep->GetInteractionState();
		int state = self->WidgetRep->ComputeInteractionState(X,Y);
		int changed;
		// Determine if we are near the end points or the line
		if ( state == vtkProbeRepresentation3D::Outside )
		{
			changed = self->RequestCursorShape(VTK_CURSOR_DEFAULT);
		}
		else //must be near something
		{
			changed = self->RequestCursorShape(VTK_CURSOR_HAND);
			if ( state == vtkProbeRepresentation3D::OnP1 )
			{
				self->Point1Widget->SetEnabled(1);
			}
			else if ( state == vtkProbeRepresentation3D::OnP2 )
			{
				self->Point2Widget->SetEnabled(1);
			}
			else //if ( state == vtkLineRepresentation::OnLine )
			{
				self->LineHandle->SetEnabled(1);
				changed = 1; //movement along the line always needs render
			}
		}
		self->Interactor->Enable(); //avoid extra renders
		if ( changed || oldState != state )
		{
			self->Render();
		}
	}
	else //if ( self->WidgetState == vtkLineWidget2::Active )
	{
		// moving something
		double e[2];
		e[0] = static_cast<double>(X);
		e[1] = static_cast<double>(Y);
		self->InvokeEvent(vtkCommand::MouseMoveEvent,NULL); //handles observe this
		reinterpret_cast<vtkProbeRepresentation3D*>(self->WidgetRep)->
			WidgetInteraction(e);
		self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
		self->EventCallbackCommand->SetAbortFlag(1);
		self->Render();
	}
}

//----------------------------------------------------------------------
void vtkProbeWidget::EndSelectAction(vtkAbstractWidget *w)
{
	vtkProbeWidget *self = reinterpret_cast<vtkProbeWidget*>(w);
	if ( self->WidgetState == vtkProbeWidget::Start )
	{
		return;
	}

	// Return state to not active
	self->WidgetState = vtkProbeWidget::Start;
	self->ReleaseFocus();
	self->InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL); //handles observe this
	self->EventCallbackCommand->SetAbortFlag(1);
	self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
	self->Superclass::EndInteraction();
	self->Render();
}

//----------------------------------------------------------------------------
void vtkProbeWidget::SetProcessEvents(int pe)
{
	this->Superclass::SetProcessEvents(pe);

	this->Point1Widget->SetProcessEvents(pe);
	this->Point2Widget->SetProcessEvents(pe);
	this->LineHandle->SetProcessEvents(pe);
}

//---------------------------------------------------------------------------
void vtkProbeWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------
void vtkProbeWidget::CreateDefaultRepresentation()
{
	if ( ! this->WidgetRep )
	{
		this->WidgetRep = vtkProbeRepresentation3D::New();
	}
	reinterpret_cast<vtkProbeRepresentation3D*>(this->WidgetRep)->
		InstantiateHandleRepresentation();
}
