/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/

#pragma once

// Qt includes
#include <QObject>

// STK includes
#include "stkRoboticSurgeryExport.h"

class stkRobotServerPrivate;
class STK_ROBOTICSURGERY_EXPORT stkRobotServer : public QObject
{
	Q_OBJECT

public:
	stkRobotServer(QObject *parent = 0);
	~stkRobotServer();

	QString getServerIp();
	void StartServer(int port);
	void SendJointPosition(double x, double y,double z,double ax,double ay,double az);

signals:
	void newMessage(QString msg);
	void newConnection(QString clientIp);

public slots:
	void newConnection();
	void ReadMessage();

protected:
	QScopedPointer<stkRobotServerPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkRobotServer);
	Q_DISABLE_COPY(stkRobotServer);	
};
