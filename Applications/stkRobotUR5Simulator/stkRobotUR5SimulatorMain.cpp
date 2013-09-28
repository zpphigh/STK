#include "stkRobotUR5SimulatorWidget.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkRobotUR5SimulatorWidget w;
	w.show();
	return a.exec();
}
