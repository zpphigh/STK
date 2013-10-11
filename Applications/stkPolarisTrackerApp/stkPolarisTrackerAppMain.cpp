#include <QtGui>
#include <QtGui/QApplication>

#include "stkPolarisTracker.h"
#include "stkPolarisTrackerWidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkPolarisTrackerWidget w;
	w.show();
	return a.exec();
}
