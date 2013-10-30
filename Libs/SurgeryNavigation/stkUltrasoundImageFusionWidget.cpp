#include "stkUltrasoundImageFusionWidget.h"
#include "ui_stkUltrasoundImageFusionWidget.h"

class stkUltrasoundImageFusionWidgetPrivate : public Ui_stkUltrasoundImageFusionWidget
{
public:
};



stkUltrasoundImageFusionWidget::stkUltrasoundImageFusionWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkUltrasoundImageFusionWidgetPrivate)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->setupUi(this);
}

stkUltrasoundImageFusionWidget::~stkUltrasoundImageFusionWidget()
{

}
