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


void stkSlicerDataWidget::on_LoadSlicerSceneButton_clicked()
{
	Q_D(stkSlicerDataWidget);
}


void stkSlicerDataWidget::on_SaveSlicerSceneButton_clicked()
{
	Q_D(stkSlicerDataWidget);
}

void stkSlicerDataWidget::on_CloseSlicerSceneButton_clicked()
{
	Q_D(stkSlicerDataWidget);

}

void stkSlicerDataWidget::on_SlicerSettingsButton_clicked()
{
	Q_D(stkSlicerDataWidget);

}
