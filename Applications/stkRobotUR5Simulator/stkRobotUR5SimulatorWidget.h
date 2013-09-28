#ifndef STKROBOTUR5SIMULATORWIDGET_H
#define STKROBOTUR5SIMULATORWIDGET_H

#include <QtGui/QWidget>
#include "ui_stkRobotUR5SimulatorWidget.h"

class stkRobotUR5SimulatorWidget : public QWidget
{
	Q_OBJECT

public:
	stkRobotUR5SimulatorWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
	~stkRobotUR5SimulatorWidget();

private:
	Ui::stkRobotUR5SimulatorWidgetClass ui;
};

#endif // STKROBOTUR5SIMULATORWIDGET_H
