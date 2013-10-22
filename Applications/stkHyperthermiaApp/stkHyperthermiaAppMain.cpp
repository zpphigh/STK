#include <QtGui>
#include <QtGui/QApplication>

#include "stkThermalField.h"
#include "stkSurgeryRoute.h"
#include "stkMicrowaveRoute.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkThermalField* field = new stkThermalField;
	stkSurgeryRoute* route = new stkSurgeryRoute;
	stkMicrowaveRoute* microute = new stkMicrowaveRoute;
	return a.exec();
}
