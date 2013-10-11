#include <QTimer>
#include "stkPolarisTrackerWidget.h"
#include "ui_stkPolarisTrackerWidget.h"

#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"

#include "itkCommand.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"


class stkPolarisTrackerWidgetPrivate : public Ui_stkPolarisTrackerWidget
{
public:
	stkPolarisTracker* tracker;

	QTimer* PulseGeneratorTimer;
};

stkPolarisTrackerWidget::stkPolarisTrackerWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkPolarisTrackerWidgetPrivate)
{
	Q_D(stkPolarisTrackerWidget);
	d->setupUi(this);

	//required by IGSTK for the time stamping used by the tracker
	igstk::RealTimeClock::Initialize();

	d->tracker = new stkPolarisTracker;

	d->PulseGeneratorTimer = new QTimer(this);
	connect(d->PulseGeneratorTimer,SIGNAL(timeout()), this, SLOT(onPulseTimerout()));
	

	QStringList header; 
	header<<QString::fromLocal8Bit("Name")<<QString::fromLocal8Bit("Status")<<QString::fromLocal8Bit("Tx")<<QString::fromLocal8Bit("Ty")<<QString::fromLocal8Bit("Tz")
		<<QString::fromLocal8Bit("Qx")<<QString::fromLocal8Bit("Qy")<<QString::fromLocal8Bit("Qz")<<QString::fromLocal8Bit("Qw"); 
	d->toolsTableWidget->setColumnCount(9);
	d->toolsTableWidget->setHorizontalHeaderLabels(header); 
	d->toolsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->toolsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); 
	d->toolsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	d->toolsTableWidget->setRowCount(4);
	for (int i = 0 ; i < 9 ;i++){
		d->toolsTableWidget->setColumnWidth(i,70);
	}
}

stkPolarisTrackerWidget::~stkPolarisTrackerWidget()
{

}

void stkPolarisTrackerWidget::on_connectServerButton_clicked()
{
	Q_D(stkPolarisTrackerWidget);
	QString serverIP = d->serverIPEdit->text();
	int serverPort = d->serverPortEdit->text().toInt();
	d->tracker->ConnectServer(serverIP.toAscii().data(),serverPort);
}

void stkPolarisTrackerWidget::on_openDeviceButton_clicked()
{
	Q_D(stkPolarisTrackerWidget);

	int comPort = static_cast<int> ( d->comPortChoiceComboBox->currentIndex());
	QString sromFileName = d->romFileNameEdit->text();

	if( sromFileName.isEmpty())	return;	

	float offsetX = d->tool1_OffsetXEdit->text().toDouble();
	float offsetY = d->tool1_OffsetYEdit->text().toDouble();
	float offsetZ = d->tool1_OffsetZEdit->text().toDouble();

	d->tracker->setComPortNum(comPort);
	d->tracker->Open();
	d->tracker->AttachTrackerTool("Tool1",sromFileName.toAscii().data());
	
	stkPolarisTrackerTool* tool = dynamic_cast<stkPolarisTrackerTool*>(d->tracker->GetTrackerTool("Tool1"));
	tool->SetSensorOffset(offsetX,offsetY,offsetZ);

	d->tracker->StartTracking();

	d->PulseGeneratorTimer->start(20);
}

void stkPolarisTrackerWidget::on_closeDeviceButton_clicked()
{

}

void stkPolarisTrackerWidget::on_startTrackingButton_clicked()
{

}

void stkPolarisTrackerWidget::on_stopTrackingButton_clicked()
{

}


void stkPolarisTrackerWidget::onPulseTimerout()
{
	//igstk::PulseGenerator::CheckTimeouts();
	Q_D(stkPolarisTrackerWidget);
	d->tracker->TrackAndSendData();

	stkPolarisTrackerTool* tool1 = dynamic_cast<stkPolarisTrackerTool*> (d->tracker->GetTrackerTool("Tool1"));
	if (tool1->dataValid())
	{
		float pos[3];
		float quat[4];
		tool1->GetPosition(pos,quat);

		d->toolsTableWidget->setItem(0,0, new QTableWidgetItem(QString::fromLocal8Bit("Tool1")));
		d->toolsTableWidget->setItem(0,1, new QTableWidgetItem(QString::fromLocal8Bit("Normal")));
		d->toolsTableWidget->setItem(0,2, new QTableWidgetItem(QString::number(pos[0])));
		d->toolsTableWidget->setItem(0,3, new QTableWidgetItem(QString::number(pos[1])));
		d->toolsTableWidget->setItem(0,4, new QTableWidgetItem(QString::number(pos[2])));
		d->toolsTableWidget->setItem(0,5, new QTableWidgetItem(QString::number(quat[0])));
		d->toolsTableWidget->setItem(0,6, new QTableWidgetItem(QString::number(quat[1])));	
		d->toolsTableWidget->setItem(0,7, new QTableWidgetItem(QString::number(quat[2])));
		d->toolsTableWidget->setItem(0,8, new QTableWidgetItem(QString::number(quat[3])));
	}


}