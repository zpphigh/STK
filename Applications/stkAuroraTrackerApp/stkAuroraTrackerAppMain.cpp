#include <QtGui>
#include <QtGui/QApplication>

#include "stkAuroraTracker.h"
#include "stkAuroraTrackerWidget.h"
#include "stkFiducialMarkerRegistrationWidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkFiducialMarkerRegistrationWidget w;
	w.show();
	return a.exec();
}
