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

	this->rtImageShift[0] = 0;
	this->rtImageShift[1] = 0;
	this->rtImageShift[2] = 0;
	this->rtImageRotate[0] = 0.0;
	this->rtImageRotate[1] = 0.0;
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



	//start timer
	this->connect(&d->importDataAndEventsTimer, SIGNAL(timeout()),  this, SLOT(importDataAndEvents()));
	d->importDataAndEventsTimer.start(10);
}

stkUltrasoundImageFusionWidget::~stkUltrasoundImageFusionWidget()
{

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
