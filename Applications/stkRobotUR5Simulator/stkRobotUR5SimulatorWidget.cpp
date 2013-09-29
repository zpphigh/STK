#include "stkRobotUR5SimulatorWidget.h"
#include "ui_stkRobotUR5SimulatorWidget.h"
#include "stkRobotSimulator.h"


class stkRobotUR5SimulatorWidgetPrivate : public Ui_stkRobotUR5SimulatorWidgetClass
{
public:

};



stkRobotUR5SimulatorWidget::stkRobotUR5SimulatorWidget(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),d_ptr(new stkRobotUR5SimulatorWidgetPrivate)
{
	Q_D(stkRobotUR5SimulatorWidget);
	d->setupUi(this);


}

stkRobotUR5SimulatorWidget::~stkRobotUR5SimulatorWidget()
{

}



void stkRobotUR5SimulatorWidget::on_connectServerPushButton_clicked()
{

}

void stkRobotUR5SimulatorWidget::on_sendMsgPushButton_clicked()
{

}
