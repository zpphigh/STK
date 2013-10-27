#ifndef STKFIDUCIALMARKERREGISTRATIONWIDGET_H
#define STKFIDUCIALMARKERREGISTRATIONWIDGET_H

#include <QWidget>
#include "ui_stkFiducialMarkerRegistrationWidget.h"

class stkFiducialMarkerRegistrationWidget : public QWidget
{
	Q_OBJECT

public:
	stkFiducialMarkerRegistrationWidget(QWidget *parent = 0);
	~stkFiducialMarkerRegistrationWidget();

private:
	Ui::stkFiducialMarkerRegistrationWidget ui;
};

#endif // STKFIDUCIALMARKERREGISTRATIONWIDGET_H
