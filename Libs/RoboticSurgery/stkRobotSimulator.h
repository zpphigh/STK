/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkRoboticSurgeryExport.h"

class stkRobotSimulatorPrivate;
class STK_ROBOTICSURGERY_EXPORT stkRobotSimulator : public QObject
{
	Q_OBJECT

public:
	stkRobotSimulator(QObject *parent = 0);
	~stkRobotSimulator();

	bool ConnectServer(QString ip, int port);  
	void SendJointPosition(double x, double y,double z,double ax,double ay,double az);

signals:
	void newMessage(QString msg);

public slots:
	void ReadMessage();
	

protected:
	QScopedPointer<stkRobotSimulatorPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkRobotSimulator);
	Q_DISABLE_COPY(stkRobotSimulator);	
};
