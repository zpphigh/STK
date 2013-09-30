#include "stkRobotUR5SimulatorWidget.h"
#include "ui_stkRobotUR5SimulatorWidget.h"
#include "stkRobotSimulator.h"


class stkRobotUR5SimulatorWidgetPrivate : public Ui_stkRobotUR5SimulatorWidgetClass
{
public:
	stkRobotSimulator* RobotSim;
};



stkRobotUR5SimulatorWidget::stkRobotUR5SimulatorWidget(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),d_ptr(new stkRobotUR5SimulatorWidgetPrivate)
{
	Q_D(stkRobotUR5SimulatorWidget);
	d->setupUi(this);
	d->RobotSim = NULL;

}

stkRobotUR5SimulatorWidget::~stkRobotUR5SimulatorWidget()
{

}



void stkRobotUR5SimulatorWidget::on_connectServerPushButton_clicked()
{
	Q_D(stkRobotUR5SimulatorWidget);
	QString serverIp = d->serverIpEdit->text();
	int serverPort = d->serverPortEdit->text().toInt();

	d->RobotSim = new stkRobotSimulator(this);
	if(!d->RobotSim->ConnectServer(serverIp,serverPort))
		return;

	d->serverStatusEdit->setText(QString("Robot Server from ")+serverIp+QString(":")+QString::number(serverPort)+QString(" is connected ..."));

	connect(d->RobotSim, SIGNAL(newMessage(QString)), d->serverStatusEdit,SLOT(setText(QString)));
}

void stkRobotUR5SimulatorWidget::on_sendMsgPushButton_clicked()
{
	Q_D(stkRobotUR5SimulatorWidget);

	double x = d->sendPoseXEdit->text().toFloat();
	double y = d->sendPoseYEdit->text().toFloat();
	double z = d->sendPoseZEdit->text().toFloat();
	double ax = d->sendPoseAXEdit->text().toFloat();
	double ay = d->sendPoseAYEdit->text().toFloat();
	double az = d->sendPoseAZEdit->text().toFloat();

	d->RobotSim->SendJointPosition(x,y,z,ax,ay,az);
}
