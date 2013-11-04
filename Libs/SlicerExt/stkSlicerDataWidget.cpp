#include "stkSlicerDataWidget.h"
#include "ui_stkSlicerDataWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerIOManager.h"
#include "vtkMRMLScene.h"

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


void stkSlicerDataWidget::on_FileAddVolumeButton_clicked()
{
	qSlicerApplication::application()->ioManager()->openAddVolumesDialog();
	emit VolumeDataAdded();
}


void stkSlicerDataWidget::on_SaveSlicerSceneButton_clicked()
{
	qSlicerApplication::application()->ioManager()->openSaveDataDialog();
}

void stkSlicerDataWidget::on_CloseSlicerSceneButton_clicked()
{
	qSlicerCoreApplication::application()->mrmlScene()->Clear(false);
}

void stkSlicerDataWidget::on_SlicerSettingsButton_clicked()
{
	qSlicerApplication::application()->settingsDialog()->exec();
}
