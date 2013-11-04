#include "stkSlicerVolumeRenderingWidget.h"
#include "ui_stkSlicerVolumeRenderingWidget.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"

class stkSlicerVolumeRenderingWidgetPrivate : public Ui_stkSlicerVolumeRenderingWidget
{
public:
	vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};


stkSlicerVolumeRenderingWidget::stkSlicerVolumeRenderingWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerVolumeRenderingWidgetPrivate)
{
	Q_D(stkSlicerVolumeRenderingWidget);
	d->setupUi(this);

	d->VolumeRenderingDisplayNode = 0;
}

stkSlicerVolumeRenderingWidget::~stkSlicerVolumeRenderingWidget()
{

}


/// Set the MRML node of interest
void stkSlicerVolumeRenderingWidget::setMRMLVolumeNode(vtkMRMLNode* node)
{
	Q_D(stkSlicerVolumeRenderingWidget);

	vtkMRMLScalarVolumeNode* scalarVolumeNode =	vtkMRMLScalarVolumeNode::SafeDownCast(node);
}