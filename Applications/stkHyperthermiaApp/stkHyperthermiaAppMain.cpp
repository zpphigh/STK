#include <QtGui>
#include <QtGui/QApplication>

#include "stkThermalField.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkThermalField* field = new stkThermalField;
	return a.exec();
}
