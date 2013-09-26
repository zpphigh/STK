/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/

#include "stkRobotServer.h"

#include <QtNetwork>

class stkRobotServerPrivate
{
public:
	QTcpServer* TcpServer;
	QTcpSocket* TcpSocket;

	void SendMessage(QString msg);
};


void stkRobotServerPrivate::SendMessage(QString msg) 
{  
	if(!TcpServer || !TcpSocket)
		return;
	
	TcpSocket->write(msg.toStdString().c_str(),strlen(msg.toStdString().c_str()));
}  



stkRobotServer::stkRobotServer(QObject *parent)
: QObject(parent),d_ptr(new stkRobotServerPrivate)
{
	Q_D(stkRobotServer);
	d->TcpServer = NULL;
	d->TcpSocket = NULL;

}

stkRobotServer::~stkRobotServer()
{

}

QString stkRobotServer::getServerIp()
{
	// find out which IP to connect to
	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
				ipAddress = ipAddressesList.at(i).toString();
				break;
		}
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

	return ipAddress;
}


void stkRobotServer::StartServer(int port)
{
	Q_D(stkRobotServer);
	d->TcpServer = new QTcpServer(this);
	d->TcpServer->listen(QHostAddress::Any,port);

	connect(d->TcpServer,SIGNAL(newConnection()),this,SLOT(newConnection()));
}



void stkRobotServer::newConnection()
{
	Q_D(stkRobotServer);
	d->TcpSocket = d->TcpServer->nextPendingConnection();
	connect(d->TcpSocket,SIGNAL(readyRead()),this,SLOT(ReadMessage()));

	QString clientIp = d->TcpSocket->peerAddress().toString();
	emit newConnection(clientIp);
}


void stkRobotServer::ReadMessage() 
{  
	Q_D(stkRobotServer);

	QByteArray qba = d->TcpSocket->readAll();  
	QString msg = QVariant(qba).toString();  
	emit newMessage(msg);
}  

void stkRobotServer::SendJointPosition(double x, double y,double z,double ax,double ay,double az)
{
	Q_D(stkRobotServer);

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


