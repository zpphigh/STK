
#include "stkTrackingErrorObserver.h"
#include "igstkTracker.h"


stkTrackingErrorObserver::stkTrackingErrorObserver(): m_ErrorOccured( false )
{
	//serial communication errors
	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::OpenPortErrorEvent().GetEventName(), "Error opening com port." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::ClosePortErrorEvent().GetEventName(), "Error closing com port." ) );

	//tracker errors
	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerOpenErrorEvent().GetEventName(),
		"Error opening tracker communication." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerInitializeErrorEvent().GetEventName(),
		"Error initializing tracker." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerStartTrackingErrorEvent().GetEventName(),
		"Error starting tracking." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerStopTrackingErrorEvent().GetEventName(),
		"Error stopping tracking." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerCloseErrorEvent().GetEventName(),
		"Error closing tracker communication." ) );

	this->m_ErrorEvent2ErrorMessage.insert(
		std::pair<std::string,std::string>(
		igstk::TrackerUpdateStatusErrorEvent().GetEventName(),
		"Error updating transformations from tracker." ) );
}

void stkTrackingErrorObserver::Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & event )
{
	std::map<std::string,std::string>::iterator it;
	std::string className = event.GetEventName();
	it = this->m_ErrorEvent2ErrorMessage.find( className );
	if( it != this->m_ErrorEvent2ErrorMessage.end() )
	{
		this->m_ErrorOccured = true;
		//fl_alert( "%s\n", (*it).second.c_str() );
		//fl_beep( FL_BEEP_ERROR );
	}
}

void stkTrackingErrorObserver::Execute(const itk::Object *caller, const itk::EventObject & event )
{
	const itk::Object * constCaller = caller;
	this->Execute( constCaller, event );
}

void stkTrackingErrorObserver::ClearError()
{
	this->m_ErrorOccured = false;
}

bool stkTrackingErrorObserver::Error()
{
	return this->m_ErrorOccured;
}

