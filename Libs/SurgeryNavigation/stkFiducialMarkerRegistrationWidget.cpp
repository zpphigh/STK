#include "stkFiducialMarkerRegistrationWidget.h"
#include "ui_stkFiducialMarkerRegistrationWidget.h"


#include <ctkVTKObject.h>

#include "qSlicerApplication.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLMarkupsNode.h"

#include "itkPoint.h"
typedef  std::vector<itk::Point<double, 3> > PointList;

class stkFiducialMarkerRegistrationWidgetPrivate : public Ui_stkFiducialMarkerRegistrationWidget
{
public:

	int NumFiducials;
	int NumFiducialsCollected;
	PointList FiducialMarkerPoints;
	PointList ToolPoints;

};


stkFiducialMarkerRegistrationWidget::stkFiducialMarkerRegistrationWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkFiducialMarkerRegistrationWidgetPrivate)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->setupUi(this);

	d->FiducialMarkerTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->FiducialMarkerTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); 
	d->FiducialMarkerTableWidget->setColumnWidth(0,50);
	d->FiducialMarkerTableWidget->setColumnWidth(1,150);
	d->FiducialMarkerTableWidget->setColumnWidth(2,150);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	this->qvtkConnect(scene,vtkMRMLScene::NodeAddedEvent, this, SLOT(updateFiducialMarkers())); 
}


stkFiducialMarkerRegistrationWidget::~stkFiducialMarkerRegistrationWidget()
{

}

void stkFiducialMarkerRegistrationWidget::on_AddFiducialMarkerToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	vtkMRMLInteractionNode* interationNode = vtkMRMLInteractionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));
	if(!interationNode)	return;

	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
	if (!selectionNode)	return;

	interationNode->SwitchToPersistentPlaceMode();

	if( d->AddFiducialMarkerToolButton->isChecked()){
		selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");
		interationNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);

	}else{
		interationNode->SwitchToViewTransformMode();
	}
}

void stkFiducialMarkerRegistrationWidget::updateFiducialMarkers()
{
	Q_D(stkFiducialMarkerRegistrationWidget);	

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	vtkMRMLNode *markupsNodeMRML = scene->GetNodeByID("vtkMRMLMarkupsFiducialNode1");
	if(!markupsNodeMRML) return;

	vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNodeMRML);
	if(!markupsNode) return;

	d->FiducialMarkerTableWidget->clearContents();

	d->NumFiducials = markupsNode->GetNumberOfMarkups();
	if ( d->NumFiducials<= 0)
		return;

	d->NumFiducialsCollected = 0;

	d->FiducialMarkerTableWidget->setRowCount(d->NumFiducials);
	d->FiducialMarkerPoints.resize(d->NumFiducials);
	d->ToolPoints.resize(d->NumFiducials);

	for (int i = 0; i < d->NumFiducials; i++)
	{		
		double coordinate[3];
		markupsNode->GetMarkupPoint(i, 0, coordinate);

		QString markupLabel = QString(markupsNode->GetNthMarkupLabel(i).c_str());
		d->FiducialMarkerTableWidget->setItem(i,0,new QTableWidgetItem(markupLabel));		

		itk::Point<double, 3> point;
		point[0] = coordinate[0];
		point[1] = coordinate[1];
		point[2] = coordinate[2];
		d->FiducialMarkerPoints[i]= point;		

		char str[50];
		sprintf(str, "%.1f,%.1f,%.1f",coordinate[0],coordinate[1],coordinate[2]);

		d->FiducialMarkerTableWidget->setItem(i,1,new QTableWidgetItem(str));		
		d->FiducialMarkerTableWidget->setItem(i,2,new QTableWidgetItem("NA"));		
		d->FiducialMarkerTableWidget->setItem(i,3,new QTableWidgetItem("NA"));

	}
	d->FiducialMarkerTableWidget->selectRow(0);


}