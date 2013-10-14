
#include "stkPivotCalibrationWidget.h"
#include "ui_stkPivotCalibrationWidget.h"
#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkPivotCalibration.h"
#include <QQuaternion>


typedef itk::MemberCommand<stkPivotCalibrationWidgetPrivate>  InitializationObserverType;
typedef itk::MemberCommand<stkPivotCalibrationWidgetPrivate>  CalibrationObserverType;

igstkObserverMacro( TransformTo, igstk::CoordinateSystemTransformToEvent, igstk::CoordinateSystemTransformToResult )
igstkObserverMacro( PivotPoint, igstk::PointEvent, igstk::EventHelperType::PointType )
igstkObserverMacro( RMSE, igstk::DoubleTypeEvent, igstk::EventHelperType::DoubleType )

class stkPivotCalibrationWidgetPrivate : public Ui_stkPivotCalibrationWidget
{
public:
	//the igstk class that actually does everything
	igstk::PivotCalibration::Pointer m_pivotCalibration;
	//delay before data acquisition starts [milliseconds]
	unsigned int m_delay;
	//description of the tool we want to calibrate
	std::string m_currentToolInformation;

	//accumulate the calibration information in this stream and
	//then display to the user
	std::ostringstream m_calibrationInformationStream;


	//pivot calibration initialization observer
	void OnInitializationEvent( itk::Object *caller, const itk::EventObject & event );
	InitializationObserverType::Pointer m_InitializationObserver; 

	//pivot calibration events (start, progress, end) observer
	void OnCalibrationEvent( itk::Object *caller, const itk::EventObject & event );
	CalibrationObserverType::Pointer m_CalibrationObserver;  

	//pivot calibration get transform observer
	TransformToObserver::Pointer m_TransformToObserver;

	//pivot calibration get pivot point observer
	PivotPointObserver::Pointer m_PivotPointObserver;

	//pivot calibration get RMSE observer
	RMSEObserver::Pointer m_RMSEObserver;

	bool m_HasTheResult;
	double m_CalibOffset[3];
};



stkPivotCalibrationWidget::stkPivotCalibrationWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkPivotCalibrationWidgetPrivate)
{
	Q_D(stkPivotCalibrationWidget);
	d->setupUi(this);

	d->computeCalibrationButton->setEnabled(false);

	d->m_delay = 0;

	//create the class that actually does all the work
	d->m_pivotCalibration = igstk::PivotCalibration::New();



	//add observer for initialization events
	d->m_InitializationObserver = InitializationObserverType::New();
	d->m_InitializationObserver->SetCallbackFunction(d, &stkPivotCalibrationWidgetPrivate::OnInitializationEvent );
	d->m_pivotCalibration->AddObserver(	igstk::PivotCalibration::InitializationFailureEvent(), d->m_InitializationObserver );
	d->m_pivotCalibration->AddObserver( igstk::PivotCalibration::InitializationSuccessEvent(), d->m_InitializationObserver );


	//add observer for the events generated during pivot calibration
	d->m_CalibrationObserver = CalibrationObserverType::New();
	d->m_CalibrationObserver->SetCallbackFunction(d, 	&stkPivotCalibrationWidgetPrivate::OnCalibrationEvent );

	d->m_pivotCalibration->AddObserver( igstk::PivotCalibration::DataAcquisitionEvent(),d->m_CalibrationObserver );
	d->m_pivotCalibration->AddObserver( igstk::PivotCalibration::DataAcquisitionEndEvent(),	d->m_CalibrationObserver );
	d->m_pivotCalibration->AddObserver( igstk::PivotCalibration::CalibrationSuccessEvent(),	d->m_CalibrationObserver );
	d->m_pivotCalibration->AddObserver( igstk::PivotCalibration::CalibrationFailureEvent(),	d->m_CalibrationObserver );

	d->m_TransformToObserver = TransformToObserver::New();
	d->m_PivotPointObserver = PivotPointObserver::New();
	d->m_RMSEObserver = RMSEObserver::New();

	//settings for the stream that accumulates the calibration information
	d->m_calibrationInformationStream.precision(3);
	d->m_calibrationInformationStream.setf(ios::fixed); 

	//是否有计算结果
	d->m_HasTheResult = false;
}

stkPivotCalibrationWidget::~stkPivotCalibrationWidget()
{

}

void stkPivotCalibrationWidget::RequestInitialize( unsigned int n, igstk::TrackerTool::Pointer trackerTool )
{
	Q_D(stkPivotCalibrationWidget);

	//show the description of the current tool on the UI
	const std::string trackerToolIdentifier = trackerTool->GetTrackerToolIdentifier();

	d->toolDescriptionLabel->setText(trackerToolIdentifier.c_str());

	//try to initialize  
	d->m_pivotCalibration->RequestInitialize( n, trackerTool );
}



void stkPivotCalibrationWidget::RequestSetDelay( unsigned int delayInSeconds )
{
	Q_D(stkPivotCalibrationWidget);
	d->m_delay = delayInSeconds*1000;
}

void stkPivotCalibrationWidget::RequestCalibrationTransform()
{
	Q_D(stkPivotCalibrationWidget);
	d->m_pivotCalibration->RequestCalibrationTransform();
}


void stkPivotCalibrationWidget::RequestPivotPoint()
{
	Q_D(stkPivotCalibrationWidget);
	d->m_pivotCalibration->RequestPivotPoint();
}



void stkPivotCalibrationWidget::RequestCalibrationRMSE()
{
	Q_D(stkPivotCalibrationWidget);
	d->m_pivotCalibration->RequestCalibrationRMSE();
}


unsigned long stkPivotCalibrationWidget::AddObserver( const itk::EventObject & event, itk::Command *command )
{
	Q_D(stkPivotCalibrationWidget);

	return d->m_pivotCalibration->AddObserver( event, command );
}

unsigned long stkPivotCalibrationWidget::AddObserver( const itk::EventObject & event, itk::Command *command ) const
{
	return d_ptr->m_pivotCalibration->AddObserver( event, command );
}


void stkPivotCalibrationWidget::RemoveObserver( unsigned long observerID )
{
	Q_D(stkPivotCalibrationWidget);
	d->m_pivotCalibration->RemoveObserver( observerID );
}


void stkPivotCalibrationWidget::on_computeCalibrationButton_clicked()
{
	Q_D(stkPivotCalibrationWidget);


	std::ostringstream msg;
	d->computeCalibrationButton->setEnabled(false);


	for(unsigned int i=d->m_delay; i>0; i-=1000 )
	{
		msg.str("");
		msg<<"Data acquisition starts in "<<(int)(i/1000)<<" seconds."; 
		d->outputEdit->setPlainText(msg.str().c_str());
		d->outputEdit->update();
		igstk::PulseGenerator::Sleep(1000);
	}

	d->outputEdit->setPlainText("");

	d->m_pivotCalibration->RequestComputeCalibration();
}

void stkPivotCalibrationWidget::on_completeCalibrationButton_clicked()
{
	Q_D(stkPivotCalibrationWidget);
	if(!d->m_HasTheResult)
		return;

	emit setCalibOffset(d->m_CalibOffset[0], d->m_CalibOffset[1], d->m_CalibOffset[2]);
	this->close();
}



void stkPivotCalibrationWidgetPrivate::OnInitializationEvent( itk::Object *caller, const itk::EventObject & event )
{
	if( dynamic_cast< const igstk::PivotCalibration::InitializationSuccessEvent * > (&event) )
	{
		//activate "Calibrate" button 
		computeCalibrationButton->setEnabled(true);
	}
	else if( dynamic_cast<const igstk::PivotCalibration::InitializationFailureEvent * > (&event) )
	{
		computeCalibrationButton->setEnabled(false);
		//qDebug("%s\n", "Failed to initialize pivot calibration.\n\ heck that tool is valid.");
		//fl_beep(FL_BEEP_ERROR);
	}
}

void stkPivotCalibrationWidgetPrivate::OnCalibrationEvent( itk::Object *caller, const itk::EventObject & event )
{
	std::ostringstream msg;

	if( const igstk::PivotCalibration::DataAcquisitionEvent *daevt = dynamic_cast< const igstk::PivotCalibration::DataAcquisitionEvent * > (&event) )
	{
		progressBar->setValue( daevt->Get() * 100 );
		//Fl::check();
	}
	else if( dynamic_cast< 	const igstk::PivotCalibration::DataAcquisitionEndEvent * > (&event) )
	{
		progressBar->setValue( 0 );
	}
	else if( const igstk::PivotCalibration::CalibrationFailureEvent *evt = dynamic_cast< const igstk::PivotCalibration::CalibrationFailureEvent * > (&event) )
	{
		computeCalibrationButton->setEnabled(true);
		progressBar->setValue( 0 );

		msg.str("");
		msg<<"Calibration failed:\n\t"<<evt->Get();
		//fl_alert("%s\n", msg.str().c_str());
		//fl_beep(FL_BEEP_ERROR);
	}
	//calibration succeeded, get all the information 
	//(Transformation, Pivot Point, RMSE) and display it
	else if( dynamic_cast<const igstk::PivotCalibration::CalibrationSuccessEvent * > (&event) )
	{
		progressBar->setValue( 0 );
		m_calibrationInformationStream.str("");
		igstk::PivotCalibration* calib = dynamic_cast< igstk::PivotCalibration *> (caller);

		//get the transformation
		unsigned long observerID = calib->AddObserver(igstk::CoordinateSystemTransformToEvent(),this->m_TransformToObserver );
		calib->RequestCalibrationTransform();
		calib->RemoveObserver( observerID );
		igstk::Transform transform = this->m_TransformToObserver->GetTransformTo().GetTransform();
		igstk::Transform::VersorType v = transform.GetRotation();
		igstk::Transform::VectorType t = transform.GetTranslation();

		m_calibrationInformationStream<<"Transform:\n";
	//	m_calibrationInformationStream<<" quaternion: "<<v.GetX()<<", "<<v.GetY()<<", "<<v.GetZ()<<", "<<v.GetW()<<"\n";
		m_calibrationInformationStream<<" translation: "<<t[0]<<", "<<t[1]<<", "<<t[2]<<"\n";
		//get the pivot point
		observerID = calib->AddObserver( igstk::PointEvent(), this->m_PivotPointObserver );
		calib->RequestPivotPoint();
		calib->RemoveObserver( observerID );
		igstk::EventHelperType::PointType pnt = this->m_PivotPointObserver->GetPivotPoint();
		m_calibrationInformationStream<<"Pivot point: "<<pnt[0]<<", "<<pnt[1]<<", "<<pnt[2]<<"\n";
		//get the RMS error
		observerID = calib->AddObserver( igstk::DoubleTypeEvent(),this->m_RMSEObserver );
		calib->RequestCalibrationRMSE();
		calib->RemoveObserver( observerID );
		m_calibrationInformationStream<<"RMSE: "<<this->m_RMSEObserver->GetRMSE()<<"\n";

		outputEdit->setPlainText(m_calibrationInformationStream.str().c_str());
		m_CalibOffset[0] = t[0];
		m_CalibOffset[1] = t[1];
		m_CalibOffset[2] = t[2];
		m_HasTheResult = true;

		computeCalibrationButton->setEnabled(true);

	}
}