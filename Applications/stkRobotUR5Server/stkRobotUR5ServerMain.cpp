/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/


#include "stkRobotUR5ServerWidget.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	stkRobotUR5ServerWidget w;
	w.show();
	return a.exec();
}
