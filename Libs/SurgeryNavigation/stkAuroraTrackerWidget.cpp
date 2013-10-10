#include <QTimer>
#include "stkAuroraTrackerWidget.h"
#include "ui_stkAuroraTrackerWidget.h"

#include "stkAuroraTracker.h"
#include "stkAuroraTrackerTool.h"

#include "itkCommand.h"
#include "igstkAuroraTracker.h"
#include "igstkAuroraTrackerTool.h"


class stkAuroraTrackerWidgetPrivate : public Ui_stkAuroraTrackerWidget
{
public:
	stkAuroraTracker* tracker;

	QTimer* PulseGeneratorTimer;
};

stkAuroraTrackerWidget::stkAuroraTrackerWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkAuroraTrackerWidgetPrivate)
{
	Q_D(stkAuroraTrackerWidget);
	d->setupUi(this);

	//required by IGSTK for the time stamping used by the tracker
	igstk::RealTimeClock::Initialize();

	d->tracker = new stkAuroraTracker;

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

stkAuroraTrackerWidget::~stkAuroraTrackerWidget()
{

}

void stkAuroraTrackerWidget::on_connectServerButton_clicked()
{
	Q_D(stkAuroraTrackerWidget);
	QString serverIP = d->serverIPEdit->text();
	int serverPort = d->serverPortEdit->text().toInt();
	d->tracker->ConnectServer(serverIP.toAscii().data(),serverPort);
}

void stkAuroraTrackerWidget::on_openDeviceButton_clicked()
{
	Q_D(stkAuroraTrackerWidget);

	int comPort = static_cast<int> ( d->comPortChoiceComboBox->currentIndex());

	d->tracker->setComPortNum(comPort);
	d->tracker->Open();
	d->tracker->AttachTrackerTool("Tool0","0");
	d->tracker->AttachTrackerTool("Tool1","1");
	d->tracker->AttachTrackerTool("Tool2","2");
	d->tracker->AttachTrackerTool("Tool3","3");

	d->tracker->StartTracking();

	d->PulseGeneratorTimer->start(20);
}

void stkAuroraTrackerWidget::on_closeDeviceButton_clicked()
{

}

void stkAuroraTrackerWidget::on_startTrackingButton_clicked()
{

}

void stkAuroraTrackerWidget::on_stopTrackingButton_clicked()
{

}


void stkAuroraTrackerWidget::onPulseTimerout()
{
	//igstk::PulseGenerator::CheckTimeouts();
	Q_D(stkAuroraTrackerWidget);
	d->tracker->TrackAndSendData();

	float pos[3];
	float quat[4];

	stkAuroraTrackerTool* tool[4];
	tool[0] = dynamic_cast<stkAuroraTrackerTool*> (d->tracker->GetTrackerTool("Tool0"));
	tool[1] = dynamic_cast<stkAuroraTrackerTool*> (d->tracker->GetTrackerTool("Tool1"));
	tool[2] = dynamic_cast<stkAuroraTrackerTool*> (d->tracker->GetTrackerTool("Tool2"));
	tool[3] = dynamic_cast<stkAuroraTrackerTool*> (d->tracker->GetTrackerTool("Tool3"));

	for (int i = 0; i < 4; i++)
	{
		if (tool[i]->dataValid())
		{
			tool[i]->GetPosition(pos,quat);
			d->toolsTableWidget->setItem(i,0, new QTableWidgetItem(QString::fromLocal8Bit("Tool")+QString::number(i)));
			d->toolsTableWidget->setItem(i,1, new QTableWidgetItem(QString::fromLocal8Bit("Normal")));
			d->toolsTableWidget->setItem(i,2, new QTableWidgetItem(QString::number(pos[0])));
			d->toolsTableWidget->setItem(i,3, new QTableWidgetItem(QString::number(pos[1])));
			d->toolsTableWidget->setItem(i,4, new QTableWidgetItem(QString::number(pos[2])));
			d->toolsTableWidget->setItem(i,5, new QTableWidgetItem(QString::number(quat[0])));
			d->toolsTableWidget->setItem(i,6, new QTableWidgetItem(QString::number(quat[1])));	
			d->toolsTableWidget->setItem(i,7, new QTableWidgetItem(QString::number(quat[2])));
			d->toolsTableWidget->setItem(i,8, new QTableWidgetItem(QString::number(quat[3])));
		}
		else
		{
			d->toolsTableWidget->setItem(i,0, new QTableWidgetItem(QString::fromLocal8Bit("Tool")+QString::number(i)));
			d->toolsTableWidget->setItem(i,1, new QTableWidgetItem(QString::fromLocal8Bit("Disconnected")));
		}
	}
}