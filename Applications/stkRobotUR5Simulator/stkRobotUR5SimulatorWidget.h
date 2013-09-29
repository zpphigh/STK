

#pragma once

#include <QtGui/QWidget>
class stkRobotUR5SimulatorWidgetPrivate;
class stkRobotUR5SimulatorWidget : public QWidget
{
	Q_OBJECT

public:
	stkRobotUR5SimulatorWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
	~stkRobotUR5SimulatorWidget();

	
public slots:
	void on_connectServerPushButton_clicked();
	void on_sendMsgPushButton_clicked();

protected:
	QScopedPointer<stkRobotUR5SimulatorWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkRobotUR5SimulatorWidget);
	Q_DISABLE_COPY(stkRobotUR5SimulatorWidget);	
};
