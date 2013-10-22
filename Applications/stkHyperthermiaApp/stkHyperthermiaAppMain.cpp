#include <QtGui>
#include <QtGui/QApplication>

#include "stkThermalField.h"
#include "stkSurgeryRoute.h"
#include "stkMicrowaveRoute.h"
#include "stkHyperthermiaBase.h"
#include "stkMicrowaveHyper.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkThermalField* field = new stkThermalField;
	stkSurgeryRoute* route = new stkSurgeryRoute;
	stkMicrowaveRoute* microute = new stkMicrowaveRoute;
	stkHyperthermiaBase* thbase = new stkHyperthermiaBase;
	return a.exec();
}
