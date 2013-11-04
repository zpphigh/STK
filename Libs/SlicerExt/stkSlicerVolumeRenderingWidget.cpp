#include "stkSlicerVolumeRenderingWidget.h"
#include "ui_stkSlicerVolumeRenderingWidget.h"

class stkSlicerVolumeRenderingWidgetPrivate : public Ui_stkSlicerVolumeRenderingWidget
{
public:


};


stkSlicerVolumeRenderingWidget::stkSlicerVolumeRenderingWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerVolumeRenderingWidgetPrivate)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	d->setupUi(this);
}

stkSlicerVolumeRenderingWidget::~stkSlicerVolumeRenderingWidget()
{

}
