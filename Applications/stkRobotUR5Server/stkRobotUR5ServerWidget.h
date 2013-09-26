#ifndef STKROBOTUR5SERVERWIDGET_H
#define STKROBOTUR5SERVERWIDGET_H

#include <QtGui/QWidget>
#include "ui_stkRobotUR5ServerWidget.h"

class stkRobotUR5Server : public QWidget
{
	Q_OBJECT

public:
	stkRobotUR5Server(QWidget *parent = 0, Qt::WFlags flags = 0);
	~stkRobotUR5Server();

private:
	Ui::stkRobotUR5ServerClass ui;
};

#endif // STKROBOTUR5SERVERWIDGET_H
