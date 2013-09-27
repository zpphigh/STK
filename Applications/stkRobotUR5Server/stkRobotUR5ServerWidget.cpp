#include "stkRobotUR5ServerWidget.h"
#include "ui_stkRobotUR5ServerWidget.h"
#include "stkRobotServer.h"

class stkRobotUR5ServerWidgetPrivate :public Ui_stkRobotUR5ServerClass
{
public:
	stkRobotServer* RobotServer;
};


stkRobotUR5ServerWidget::stkRobotUR5ServerWidget(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),d_ptr(new stkRobotUR5ServerWidgetPrivate)
{
	Q_D(stkRobotUR5ServerWidget);
	d->setupUi(this);

	d->RobotServer = new stkRobotServer(this);
	QString serverIp = d->RobotServer->getServerIp();

	d->serverIpEdit->setText(serverIp);
}

stkRobotUR5ServerWidget::~stkRobotUR5ServerWidget()
{

}



void stkRobotUR5ServerWidget::on_startServerPushButton_clicked()
{
	Q_D(stkRobotUR5ServerWidget);
	int port = d->serverPortEdit->text().toInt();
	d->RobotServer->StartServer(port);

	d->serverStatusEdit->setText("Server Started, waiting for connection ...");

	connect(d->RobotServer, SIGNAL(newConnection(QString)), this,SLOT(newConnection(QString)));
	connect(d->RobotServer, SIGNAL(newMessage(QString)), this,SLOT(newMessage(QString)));
}


void stkRobotUR5ServerWidget::on_sendMsgPushButton_clicked()
{
	Q_D(stkRobotUR5ServerWidget);

	double x = d->sendPoseXEdit->text().toFloat();
	double y = d->sendPoseYEdit->text().toFloat();
	double z = d->sendPoseZEdit->text().toFloat();
	double ax = d->sendPoseAXEdit->text().toFloat();
	double ay = d->sendPoseAYEdit->text().toFloat();
	double az = d->sendPoseAZEdit->text().toFloat();

	d->RobotServer->SendJointPosition(x,y,z,ax,ay,az);
}


void stkRobotUR5ServerWidget::newConnection(QString peerIp)
{
	Q_D(stkRobotUR5ServerWidget);
	d->serverStatusEdit->setText(QString("Peer client from ") + peerIp + " is connected ...");

	connect(d->RobotServer, SIGNAL(newMessage(QString)), d->serverStatusEdit,SLOT(setText(QString)));
}
