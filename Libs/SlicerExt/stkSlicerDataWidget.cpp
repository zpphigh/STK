#include "stkSlicerDataWidget.h"
#include "ui_stkSlicerDataWidget.h"

class stkSlicerDataWidgetPrivate : public Ui_stkSlicerDataWidget
{
public:


};



stkSlicerDataWidget::stkSlicerDataWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerDataWidgetPrivate)
{
	Q_D(stkSlicerDataWidget);
	d->setupUi(this);
}

stkSlicerDataWidget::~stkSlicerDataWidget()
{

}
