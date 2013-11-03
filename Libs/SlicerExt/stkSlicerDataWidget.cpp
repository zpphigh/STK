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


void stkSlicerDataWidget::on_LoadSceneButton_clicked()
{

}


void stkSlicerDataWidget::on_SaveSceneButton_clicked()
{

}

void stkSlicerDataWidget::on_CloseSceneButton_clicked()
{

}

void stkSlicerDataWidget::on_SlicerSettingsButton_clicked()
{

}
