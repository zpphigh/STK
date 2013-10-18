#include <QTimer>
#include "stkPolarisPivotCalibrationWidget.h"
#include "ui_stkPolarisPivotCalibrationWidget.h"

#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"

#include "itkCommand.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkPivotCalibration.h"




igstkObserverMacro( TransformTo, igstk::CoordinateSystemTransformToEvent, igstk::CoordinateSystemTransformToResult )
igstkObserverMacro( RMSE, igstk::DoubleTypeEvent, igstk::EventHelperType::DoubleType )

//pivot calibration success/failure observer
typedef itk::MemberCommand<stkPolarisPivotCalibrationWidgetPrivate> CalibrationSuccessFailureObserverType;
typedef itk::MemberCommand<stkPolarisPivotCalibrationWidgetPrivate> WriteFailureObserverType;

class stkPolarisPivotCalibrationWidgetPrivate : public Ui_stkPolarisPivotCalibrationWidget
{
public:
	stkPolarisTracker* tracker;

	//pivot calibration get transform observer
	TransformToObserver::Pointer m_TransformToObserver;

	//pivot calibration get RMSE observer
	RMSEObserver::Pointer m_RMSEObserver;


	void OnCalibrationSuccessFailureEvent( itk::Object *caller, const itk::EventObject & event );
	CalibrationSuccessFailureObserverType::Pointer m_CalibrationSuccessFailureObserver;  

	//observer for write failure event
	void OnWriteFailureEvent( itk::Object *caller, const itk::EventObject & event );



	QTimer* PulseGeneratorTimer;
};



stkPolarisPivotCalibrationWidget::stkPolarisPivotCalibrationWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkPolarisPivotCalibrationWidgetPrivate)
{
	Q_D(stkPolarisPivotCalibrationWidget);
	d->setupUi(this);

	d->tracker = new stkPolarisTracker;
	
	//required by IGSTK for the time stamping used by the tracker
	igstk::RealTimeClock::Initialize();

	d->PulseGeneratorTimer = new QTimer(this);
	connect(d->PulseGeneratorTimer,SIGNAL(timeout()), this, SLOT(onPulseTimerout()));
	d->PulseGeneratorTimer->start(20);


	//create transform observer
	d->m_TransformToObserver = TransformToObserver::New();
	//create RMSE observer
	d->m_RMSEObserver = RMSEObserver::New();

	//create calibration success and failure observer
	d->m_CalibrationSuccessFailureObserver = CalibrationSuccessFailureObserverType::New();  
	d->m_CalibrationSuccessFailureObserver->SetCallbackFunction(d, &stkPolarisPivotCalibrationWidgetPrivate::OnCalibrationSuccessFailureEvent );

	d->pivotCalibrationWidget->AddObserver( igstk::PivotCalibration::CalibrationSuccessEvent(), d->m_CalibrationSuccessFailureObserver );
	d->pivotCalibrationWidget->AddObserver( igstk::PivotCalibration::CalibrationFailureEvent(), d->m_CalibrationSuccessFailureObserver );
}

stkPolarisPivotCalibrationWidget::~stkPolarisPivotCalibrationWidget()
{
	Q_D(stkPolarisPivotCalibrationWidget);

}



void stkPolarisPivotCalibrationWidget::onPulseTimerout()
{
	igstk::PulseGenerator::CheckTimeouts();
}

void stkPolarisPivotCalibrationWidget::on_initializeButton_clicked()
{
	Q_D(stkPolarisPivotCalibrationWidget);

	int comPort = static_cast<int> ( d->comPortChoiceComboBox->currentIndex());
	std::string sromFileName = d->romFileNameEdit->text().toAscii().data();
	if( sromFileName.empty())	return;

	d->tracker->setComPortNum(comPort);
	d->tracker->Open();
	d->tracker->AttachTrackerTool("PivotTool",sromFileName.c_str());
	d->tracker->StartTracking();

	unsigned int delay =	static_cast<unsigned int>(  d->delaySecondsSpinBox->value() );
	unsigned int numberOfFrames =	static_cast<unsigned int>( d->numberOfSamplesSpinBox->value() );

	d->pivotCalibrationWidget->RequestSetDelay( delay );
	igstk::TrackerTool::Pointer genericTrackerTool = d->tracker->GetTrackerTool("PivotTool")->GetTrackerToolPointer();
	d->pivotCalibrationWidget->RequestInitialize(numberOfFrames,genericTrackerTool );
}


void stkPolarisPivotCalibrationWidget::on_saveResultButton_clicked()
{
	Q_D(stkPolarisPivotCalibrationWidget);

}

void stkPolarisPivotCalibrationWidgetPrivate::OnCalibrationSuccessFailureEvent( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
{
	if( dynamic_cast< const igstk::PivotCalibration::CalibrationSuccessEvent * > 
		(&event) ) 
	{ 
		saveResultButton->setEnabled(true);
	}
	else if( dynamic_cast< const igstk::PivotCalibration::CalibrationFailureEvent * > (&event) )
	{
		saveResultButton->setEnabled(false);
	}
}


void stkPolarisPivotCalibrationWidgetPrivate::OnWriteFailureEvent( itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
{
	//fl_alert("%s\n", "Failed writing calibration data to file." );
	//fl_beep( FL_BEEP_ERROR );
}

