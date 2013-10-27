#include "stkFiducialMarkerRegistrationWidget.h"
#include "ui_stkFiducialMarkerRegistrationWidget.h"


class stkFiducialMarkerRegistrationWidgetPrivate : public Ui_stkFiducialMarkerRegistrationWidget
{
public:

	QTimer* PulseGeneratorTimer;
};


stkFiducialMarkerRegistrationWidget::stkFiducialMarkerRegistrationWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkFiducialMarkerRegistrationWidgetPrivate)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->setupUi(this);
}


stkFiducialMarkerRegistrationWidget::~stkFiducialMarkerRegistrationWidget()
{

}
