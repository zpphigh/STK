#include <QtGui>
#include <QtGui/QApplication>

#include "stkAuroraTracker.h"
#include "stkAuroraTrackerWidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkAuroraTrackerWidget w;
	w.show();
	return a.exec();
}
