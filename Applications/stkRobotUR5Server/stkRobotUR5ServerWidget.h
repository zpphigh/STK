/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/
#pragma once
#include <QtGui/QWidget>


class stkRobotUR5ServerWidgetPrivate;
class stkRobotUR5ServerWidget : public QWidget
{
	Q_OBJECT

public:
	stkRobotUR5ServerWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
	~stkRobotUR5ServerWidget();

public slots:
	void on_sendMsgPushButton_clicked();
	void on_startServerPushButton_clicked();
	void newConnection(QString peerIp);

protected:
	QScopedPointer<stkRobotUR5ServerWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkRobotUR5ServerWidget);
	Q_DISABLE_COPY(stkRobotUR5ServerWidget);
};
