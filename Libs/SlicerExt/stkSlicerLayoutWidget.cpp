#include "stkSlicerLayoutWidget.h"
#include "ui_stkSlicerLayoutWidget.h"

//Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLLayoutNode.h"
#include "stkSlicerDisplayHelper.h"


class stkSlicerLayoutWidgetPrivate : public Ui_stkSlicerLayoutWidget
{
public:


};


stkSlicerLayoutWidget::stkSlicerLayoutWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerLayoutWidgetPrivate)
{
	Q_D(stkSlicerLayoutWidget);
	d->setupUi(this);
}

stkSlicerLayoutWidget::~stkSlicerLayoutWidget()
{

}

void stkSlicerLayoutWidget::on_SlicerLayoutConventionalViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutConventionalView);
		stkSlicerDisplayHelper::Set3DViewNodeAppearence();
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutConventionalWidescreenViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutConventionalWidescreenView);
		stkSlicerDisplayHelper::Set3DViewNodeAppearence();
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutFourUpViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutFourUpView);
		stkSlicerDisplayHelper::Set3DViewNodeAppearence();
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutOneUp3DViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutOneUp3DView);
		stkSlicerDisplayHelper::Set3DViewNodeAppearence();
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutOneUpRedSliceViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutOneUpRedSliceView);
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutOneUpYellowSliceViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutOneUpYellowSliceView);
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutOneUpGreenSliceViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutOneUpGreenSliceView);
	}
}


void stkSlicerLayoutWidget::on_SlicerLayoutDual3DViewButton_clicked()
{
	qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
	if(layoutManager)
	{
		layoutManager->setLayout(vtkMRMLLayoutNode::SlicerLayoutDual3DView);		
		stkSlicerDisplayHelper::Set3DViewNodeAppearence();
	}
}


