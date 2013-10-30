#include "stkUltrasoundImageFusionWidget.h"
#include "ui_stkUltrasoundImageFusionWidget.h"

#include <QTimer>

#include "stkMRMLHelper.h"
#include "stkMRMLIGTLServerNode.h"

#include "vtkMRMLScene.h"
#include "qMRMLNodeFactory.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLSliceLogic.h"

#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkTransform.h>
#include <vtkMRMLScalarVolumeNode.h>

enum ImageOrient{
	SLICE_RTIMAGE_NONE      = 0,
	SLICE_RTIMAGE_PERP      = 1,
	SLICE_RTIMAGE_INPLANE90 = 2,
	SLICE_RTIMAGE_INPLANE   = 3
};

class stkUltrasoundImageFusionWidgetPrivate : public Ui_stkUltrasoundImageFusionWidget
{
public:
	stkUltrasoundImageFusionWidgetPrivate();

	void UpdateSliceNode(int sliceNodeNumber, vtkMatrix4x4* transform);
	void CheckSliceNode();

	vtkSmartPointer<vtkMatrix4x4> sliceMatrix;
	vtkMRMLSliceNode *sliceNode[3];
	vtkMRMLSliceCompositeNode* sliceCompositeNode[3];
	vtkMRMLLinearTransformNode* sliceLocatorTransform;

	int   sliceOrientation[3];
	int   rtImageShift[3]; // 0 for X, 1 for Y, 2 for Z
	double   rtImageRotate[3]; // 0 for X, 1 for Y, 2 for Z


	stkMRMLIGTLServerNode* IGTLImageServerNode;
	QTimer importDataAndEventsTimer;
};

stkUltrasoundImageFusionWidgetPrivate::stkUltrasoundImageFusionWidgetPrivate()
{

	sliceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	sliceNode[0] = NULL;
	sliceNode[1] = NULL;
	sliceNode[2] = NULL;
	sliceCompositeNode[0] = NULL;
	sliceCompositeNode[1] = NULL;
	sliceCompositeNode[2] = NULL;	

	this->sliceOrientation[0] = SLICE_RTIMAGE_PERP;
	this->sliceOrientation[1] = SLICE_RTIMAGE_PERP;//SLICE_RTIMAGE_INPLANE;
	this->sliceOrientation[2] = SLICE_RTIMAGE_PERP;//SLICE_RTIMAGE_INPLANE90;

	this->rtImageShift[0] = -185;
	this->rtImageShift[1] = 0;
	this->rtImageShift[2] = 21;
	this->rtImageRotate[0] = 0.0;
	this->rtImageRotate[1] = 1.6;
	this->rtImageRotate[2] = 0.0;

	sliceLocatorTransform = NULL;
}


void stkUltrasoundImageFusionWidgetPrivate::CheckSliceNode()
{
	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	if (this->sliceNode[0] == NULL)
	{
		this->sliceNode[0] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeRed"));
		//this->sliceNode[0]->SetFieldOfView(255,255,this->sliceNode[0]->GetFieldOfView()[2]);
		this->sliceCompositeNode[0] = vtkMRMLSliceLogic::GetSliceCompositeNode(this->sliceNode[0]);
		this->sliceCompositeNode[0]->SetSliceIntersectionVisibility(0);
	}
	if (this->sliceNode[1] == NULL)
	{
		this->sliceNode[1] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeYellow"));
		//this->sliceNode[1]->SetFieldOfView(255,255,this->sliceNode[1]->GetFieldOfView()[2]);
		this->sliceCompositeNode[1] = vtkMRMLSliceLogic::GetSliceCompositeNode(this->sliceNode[1]);
		this->sliceCompositeNode[1]->SetSliceIntersectionVisibility(0);
	}
	if (this->sliceNode[2] == NULL)
	{
		this->sliceNode[2] = vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNodeGreen"));
		//this->sliceNode[2]->SetFieldOfView(255,255,this->sliceNode[2]->GetFieldOfView()[2]);
		this->sliceCompositeNode[2] = vtkMRMLSliceLogic::GetSliceCompositeNode(this->sliceNode[2]);
		this->sliceCompositeNode[2]->SetSliceIntersectionVisibility(0);
	}
}


void stkUltrasoundImageFusionWidgetPrivate::UpdateSliceNode(int sliceNodeNumber, vtkMatrix4x4* transform)
{
//  CheckSliceNode();

  float tx = transform->Element[0][0];
  float ty = transform->Element[1][0];
  float tz = transform->Element[2][0];
  /*
    float sx = transform->GetElement(0, 1);
    float sy = transform->GetElement(1, 1);
    float sz = transform->GetElement(2, 1);
  */
  float nx = transform->Element[0][2];
  float ny = transform->Element[1][2];
  float nz = transform->Element[2][2];
  float px = transform->Element[0][3];
  float py = transform->Element[1][3];
  float pz = transform->Element[2][3];

  if (strcmp(this->sliceNode[sliceNodeNumber]->GetOrientationString(), "Axial") == 0)
    {
      this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
    }
  else if (strcmp(this->sliceNode[sliceNodeNumber]->GetOrientationString(), "Sagittal") == 0)
    {
      this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
    }
  else if (strcmp(this->sliceNode[sliceNodeNumber]->GetOrientationString(), "Coronal") == 0)
    {
      this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
    }
  else if (strcmp(this->sliceNode[sliceNodeNumber]->GetOrientationString(), "Reformat") == 0)
    {
		if (this->sliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_PERP)
		{
			this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);
		}
		else if (this->sliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE)
		{
			this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);//Orientation设为0，与red一致
		}
		else if (this->sliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE90)
		{
			this->sliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);//Orientation设为0
		}
    }

  this->sliceNode[sliceNodeNumber]->UpdateMatrices();
}







stkUltrasoundImageFusionWidget::stkUltrasoundImageFusionWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkUltrasoundImageFusionWidgetPrivate)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->setupUi(this);

	d->IGTLImageServerNode = NULL;

	d->XShiftSlider->setValue(d->rtImageShift[0]);
	d->YShiftSlider->setValue(d->rtImageShift[1]);
	d->ZShiftSlider->setValue(d->rtImageShift[2]);
	d->XShiftLabel->setText(QString::number(d->rtImageShift[0]));
	d->YShiftLabel->setText(QString::number(d->rtImageShift[1]));
	d->ZShiftLabel->setText(QString::number(d->rtImageShift[2]));
	d->XRotateSlider->setValue(d->rtImageRotate[0]*10);
	d->YRotateSlider->setValue(d->rtImageRotate[1]*10);
	d->ZRotateSlider->setValue(d->rtImageRotate[2]*10);
	d->XRotateLabel->setText(QString("%1").arg(d->rtImageRotate[0]));
	d->YRotateLabel->setText(QString("%1").arg(d->rtImageRotate[1]));
	d->ZRotateLabel->setText(QString("%1").arg(d->rtImageRotate[2]));


	//信号和槽 当滑动条的值发生改变时，即产生一个valueChanged(int)信号 设置QLineEdit控件的显示文本   
	connect(d->XShiftSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageShiftX(int)));  
	connect(d->YShiftSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageShiftY(int)));  
	connect(d->ZShiftSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageShiftZ(int)));  
	connect(d->XRotateSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageRotateX(int)));  
	connect(d->YRotateSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageRotateY(int)));  
	connect(d->ZRotateSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRTImageRotateZ(int))); 
	connect(d->ImageFusionSlider, SIGNAL(valueChanged(int)), this, SLOT(SetImageFusionOpacity(int))); 

	//start timer
	this->connect(&d->importDataAndEventsTimer, SIGNAL(timeout()),  this, SLOT(importDataAndEvents()));
	d->importDataAndEventsTimer.start(10);
}

stkUltrasoundImageFusionWidget::~stkUltrasoundImageFusionWidget()
{

}


void stkUltrasoundImageFusionWidget::SetRTImageShiftX(int shift)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageShift[0] = shift;
	d->XShiftLabel->setText(QString::number(d->rtImageShift[0]));
}

void stkUltrasoundImageFusionWidget::SetRTImageShiftY(int shift)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageShift[1] = shift;
	d->YShiftLabel->setText(QString::number(d->rtImageShift[1]));
}

void stkUltrasoundImageFusionWidget::SetRTImageShiftZ(int shift)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageShift[2] = shift;
	d->ZShiftLabel->setText(QString::number(d->rtImageShift[2]));
}

void stkUltrasoundImageFusionWidget::SetRTImageRotateX(int rotate)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageRotate[0] = rotate/10.0;
	d->XRotateLabel->setText(QString("%1").arg(d->rtImageRotate[0]));
}

void stkUltrasoundImageFusionWidget::SetRTImageRotateY(int rotate)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageRotate[1] = rotate/10.0;
	d->YRotateLabel->setText(QString("%1").arg(d->rtImageRotate[1]));
}

void stkUltrasoundImageFusionWidget::SetRTImageRotateZ(int rotate)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->rtImageRotate[2] = rotate/10.0;
	d->ZRotateLabel->setText(QString("%1").arg(d->rtImageRotate[2]));
}


void stkUltrasoundImageFusionWidget::StartIGTLImageServer()
{
	Q_D(stkUltrasoundImageFusionWidget);

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	if(!d->IGTLImageServerNode)
	{
		vtkMRMLNode * node = qMRMLNodeFactory::createNode(scene, "stkMRMLIGTLServerNode");
		d->IGTLImageServerNode = stkMRMLIGTLServerNode::SafeDownCast(node);
		d->IGTLImageServerNode->DisableModifiedEventOn();
		d->IGTLImageServerNode->SetServerPort(18945);//RTImageServer使用18945端口
		d->IGTLImageServerNode->SetName("IGTLImageServer");
		d->IGTLImageServerNode->DisableModifiedEventOff();
		d->IGTLImageServerNode->InvokePendingModifiedEvent();
	}

	if( d->IGTLImageServerNode && d->IGTLImageServerNode->GetState() == stkMRMLIGTLServerNode::STATE_OFF )
	{
		d->IGTLImageServerNode->Start();
		d->IGTLImageServerNode->Modified();
	}	
}

void stkUltrasoundImageFusionWidget::importDataAndEvents()
{
	Q_D(stkUltrasoundImageFusionWidget);

	if(!d->IGTLImageServerNode)
		return;

	d->IGTLImageServerNode->ImportEventsFromEventBuffer();
	d->IGTLImageServerNode->ImportDataFromCircularBuffer();
}


void stkUltrasoundImageFusionWidget::SetImageFusionOpacity(int opacity)
{
	Q_D(stkUltrasoundImageFusionWidget);
	d->CheckSliceNode();
	if (d->sliceCompositeNode[0])
	{
		d->sliceCompositeNode[0]->SetForegroundOpacity(opacity/100.0);
	}

	if( opacity == 0){
		d->DisplayCTButton->setChecked(true);
		d->DisplayUSButton->setChecked(false);
		d->DisplayFusionButton->setChecked(false);
	}else if (opacity == 50){
		d->DisplayCTButton->setChecked(false);
		d->DisplayUSButton->setChecked(false);
		d->DisplayFusionButton->setChecked(true);
	}else if (opacity == 100){
		d->DisplayCTButton->setChecked(false);
		d->DisplayUSButton->setChecked(true);
		d->DisplayFusionButton->setChecked(false);
	}else{
		d->DisplayCTButton->setChecked(false);
		d->DisplayUSButton->setChecked(false);
		d->DisplayFusionButton->setChecked(false);
	}
}

void stkUltrasoundImageFusionWidget::on_DisplayUSButton_clicked()
{
	Q_D(stkUltrasoundImageFusionWidget);
	if(d->DisplayUSButton->isChecked()){
		d->ImageFusionSlider->setValue(100);
		d->DisplayCTButton->setChecked(false);
		d->DisplayFusionButton->setChecked(false);
	}	
}


void stkUltrasoundImageFusionWidget::on_DisplayCTButton_clicked()
{
	Q_D(stkUltrasoundImageFusionWidget);

	if(d->DisplayCTButton->isChecked()){
		d->ImageFusionSlider->setValue(0);
		d->DisplayUSButton->setChecked(false);
		d->DisplayFusionButton->setChecked(false);
	}	
}


void stkUltrasoundImageFusionWidget::on_DisplayFusionButton_clicked()
{
	Q_D(stkUltrasoundImageFusionWidget);

	if(d->DisplayFusionButton->isChecked()){
		d->ImageFusionSlider->setValue(50);
		d->DisplayUSButton->setChecked(false);
		d->DisplayCTButton->setChecked(false);
	}
}


void stkUltrasoundImageFusionWidget::UpdateSliceByLocator()
{	
	Q_D(stkUltrasoundImageFusionWidget);

	if( !d->sliceLocatorTransform)
		return;

	vtkMRMLLinearTransformNode* igtTransform = vtkMRMLLinearTransformNode::SafeDownCast(stkMRMLHelper::GetSingleMRMLNodeByName("IGTTransform"));

	if( igtTransform == NULL )
		return;

	vtkMatrix4x4::Multiply4x4(igtTransform->GetMatrixTransformToParent(), d->sliceLocatorTransform->GetMatrixTransformToParent(), d->sliceMatrix);
	vtkNew<vtkTransform> transform;
	transform->SetMatrix(d->sliceMatrix);

	//Begin手工调节标定参数，角度不需要调节，主要是调节平移
	float shiftX = d->rtImageShift[0];
	float shiftY = d->rtImageShift[1];
	float shiftZ = d->rtImageShift[2];
	transform->Translate(shiftX,shiftY,shiftZ); // step3：前面的最后左右
	//End 手工调节标定参数

	//UltrasoundTool
	transform->RotateZ(-90.0 + d->rtImageRotate[2]);
	transform->RotateY(d->rtImageRotate[1]);
	transform->RotateX(d->rtImageRotate[0]);
	transform->GetMatrix(d->sliceMatrix);

	d->UpdateSliceNode(0,d->sliceMatrix);
	d->UpdateSliceNode(1,d->sliceMatrix);
	d->UpdateSliceNode(2,d->sliceMatrix);

	vtkMRMLScalarVolumeNode* rtImageNode = vtkMRMLScalarVolumeNode::SafeDownCast(stkMRMLHelper::GetSingleMRMLNodeByName("RTImage"));
	if (rtImageNode ==NULL)
		return;

	rtImageNode->SetIJKToRASMatrix(d->sliceMatrix);
}
