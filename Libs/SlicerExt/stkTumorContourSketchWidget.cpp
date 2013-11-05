#include "stkTumorContourSketchWidget.h"

class stkTumorContourSketchWidgetPrivate : public Ui_stkTumorContourSketchWidget
{
public:

};



stkTumorContourSketchWidget::stkTumorContourSketchWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkTumorContourSketchWidgetPrivate)
{
	Q_D(stkTumorContourSketchWidget);
	d->setupUi(this);

}

stkTumorContourSketchWidget::~stkTumorContourSketchWidget()
{

}
