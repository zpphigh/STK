/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/

#include "stkRobotSimulator.h"
#include <QTcpSocket>

class stkRobotSimulatorPrivate
{
public:
	QTcpSocket* TcpSocket;

	void SendMessage(QString msg);
};


stkRobotSimulator::stkRobotSimulator(QObject *parent)
	: QObject(parent),d_ptr(new stkRobotSimulatorPrivate)
{
	Q_D(stkRobotSimulator);
	d->TcpSocket = NULL;
}

stkRobotSimulator::~stkRobotSimulator()
{

}


bool stkRobotSimulator::ConnectServer(QString ip, int port)
{
	Q_D(stkRobotSimulator);
	d->TcpSocket = new QTcpSocket(this);  
	d->TcpSocket->abort();  
	d->TcpSocket->connectToHost(ip,port);  

	connect(d->TcpSocket,SIGNAL(readyRead()),this,SLOT(ReadMessage()));  

	return true;
}


void stkRobotSimulator::ReadMessage() 
{  
	Q_D(stkRobotSimulator);
	QByteArray qba = d->TcpSocket->readAll();  
	QString msg = QVariant(qba).toString(); 
	emit newMessage(msg);
}  

void stkRobotSimulatorPrivate::SendMessage(QString msg) 
{  
	if(!TcpSocket)
		return;

	TcpSocket->write(msg.toStdString().c_str(),strlen(msg.toStdString().c_str()));  
}  

void stkRobotSimulator::SendJointPosition(double x, double y,double z,double ax,double ay,double az)
{
	Q_D(stkRobotSimulator);

	//Joint Position Format: (35, 0, -90, -45, 90, 70);
	QString position = QString("(")
		+QString::number(x)+QString(", ")
		+QString::number(y)+QString(", ")
		+QString::number(z)+QString(", ")
		+QString::number(ax)+QString(", ")
		+QString::number(ay)+QString(", ")
		+QString::number(az)+QString(")");

	d->SendMessage(position);
}
