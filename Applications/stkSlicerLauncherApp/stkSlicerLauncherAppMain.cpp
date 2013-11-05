#include <QtGui>
#include <QtGui/QApplication>

#include "stkAuroraTracker.h"
#include "stkAuroraTrackerWidget.h"
#include "qSlicerApplication.h"
#include "stkSlicerApp.h"
#include "stkSlicerHelper.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("fitark");
	QCoreApplication::setOrganizationDomain("fitark.org");
	QCoreApplication::setApplicationName("stkAuroraTrackerApp");
	QCoreApplication::setApplicationVersion("0.1");

	qSlicerApplication app(argc, argv);
	if (app.returnCode() != -1)
		return app.returnCode();

	stkSlicerHelper::SetupModules();	
	stkSlicerApp* w = new stkSlicerApp;
	stkSlicerHelper::LoadModules();
	w->show();

	return app.exec();
}
