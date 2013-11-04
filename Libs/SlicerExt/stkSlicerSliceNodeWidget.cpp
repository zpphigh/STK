#include "stkSlicerSliceNodeWidget.h"
#include "ui_stkSlicerSliceNodeWidget.h"


class stkSlicerSliceNodeWidgetPrivate : public Ui_stkSlicerSliceNodeWidget
{
public:
	
};


stkSlicerSliceNodeWidget::stkSlicerSliceNodeWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerSliceNodeWidgetPrivate)
{
	Q_D(stkSlicerSliceNodeWidget);
	d->setupUi(this);


}

stkSlicerSliceNodeWidget::~stkSlicerSliceNodeWidget()
{

}
