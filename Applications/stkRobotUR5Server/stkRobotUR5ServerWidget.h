/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/
#pragma once
#include <QtGui/QWidget>


class stkRobotUR5ServerPrivate;
class stkRobotUR5Server : public QWidget
{
	Q_OBJECT

public:
	stkRobotUR5Server(QWidget *parent = 0, Qt::WFlags flags = 0);
	~stkRobotUR5Server();

protected:
	QScopedPointer<stkRobotUR5ServerPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkRobotUR5Server);
	Q_DISABLE_COPY(stkRobotUR5Server);
};
