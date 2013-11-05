#include <QtGui/QApplication>
#include "qSlicerApplication.h"
#include "stkAuroraTrackerApp.h"
#include "stkSlicerHelper.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("FITARK");
	QCoreApplication::setOrganizationDomain("fitark.org");
	QCoreApplication::setApplicationName("FitCore");
	QCoreApplication::setApplicationVersion("3.0");

	qSlicerApplication app(argc, argv);
	if (app.returnCode() != -1)
	{
		return app.returnCode();
	}

	stkSlicerHelper::SetupModules();	
	stkAuroraTrackerApp* w = new stkAuroraTrackerApp;
	stkSlicerHelper::LoadModules();

	w->show();

	return app.exec();

}
