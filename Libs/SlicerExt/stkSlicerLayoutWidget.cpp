#include "stkSlicerLayoutWidget.h"
#include "ui_stkSlicerLayoutWidget.h"

class stkSlicerLayoutWidgetPrivate : public Ui_stkSlicerLayoutWidget
{
public:

};


stkSlicerLayoutWidget::stkSlicerLayoutWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkSlicerLayoutWidgetPrivate)
{
	Q_D(stkSlicerLayoutWidget);
	d->setupUi(this);
}

stkSlicerLayoutWidget::~stkSlicerLayoutWidget()
{

}
