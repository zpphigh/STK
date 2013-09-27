#include "stkRobotUR5ServerWidget.h"
#include "ui_stkRobotUR5ServerWidget.h"
#include "stkRobotServer.h"

class stkRobotUR5ServerPrivate :public Ui_stkRobotUR5ServerClass
{
public:

};


stkRobotUR5Server::stkRobotUR5Server(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),d_ptr(new stkRobotUR5ServerPrivate)
{
	Q_D(stkRobotUR5Server);
	d->setupUi(this);
}

stkRobotUR5Server::~stkRobotUR5Server()
{

}
