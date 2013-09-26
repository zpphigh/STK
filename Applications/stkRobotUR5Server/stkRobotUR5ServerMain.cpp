#include "stkRobotUR5ServerWidget.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkRobotUR5Server w;
	w.show();
	return a.exec();
}
