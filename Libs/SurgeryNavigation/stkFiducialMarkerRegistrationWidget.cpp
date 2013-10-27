#include "stkFiducialMarkerRegistrationWidget.h"
#include "ui_stkFiducialMarkerRegistrationWidget.h"


#include <ctkVTKObject.h>

#include "qSlicerApplication.h"

#include "vtkCollection.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLDisplayNode.h"
#include "ctkMessageBox.h"

#include "itkPoint.h"
typedef  std::vector<itk::Point<double, 3> > PointList;


#include "stkPolarisTracker.h"
#include "stkPolarisTrackerTool.h"
#include "stkAuroraTracker.h"
#include "stkAuroraTrackerTool.h"

enum TrackerType{
	TRACKER_TYPE_NONE      = 0,
	TRACKER_TYPE_ASCENSION      = 1,
	TRACKER_TYPE_POLARIS = 2,
	TRACKER_TYPE_AURORA   = 3
};


class stkFiducialMarkerRegistrationWidgetPrivate : public Ui_stkFiducialMarkerRegistrationWidget
{
public:
	int NumFiducials;
	int NumFiducialsCollected;
	PointList FiducialMarkerPoints;
	PointList ToolPoints;

	//Tracker
	int TrackerType;
	stkTracker* Tracker;
	stkTrackerTool* CalibrationTool;

};


stkFiducialMarkerRegistrationWidget::stkFiducialMarkerRegistrationWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkFiducialMarkerRegistrationWidgetPrivate)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->setupUi(this);

	d->TrackerType = TRACKER_TYPE_NONE;
	d->Tracker = NULL;
	d->CalibrationTool = NULL;

	d->FiducialMarkerTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->FiducialMarkerTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); 
	d->FiducialMarkerTableWidget->setColumnWidth(0,30);
	d->FiducialMarkerTableWidget->setColumnWidth(1,100);
	d->FiducialMarkerTableWidget->setColumnWidth(2,100);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	this->qvtkConnect(scene,vtkMRMLScene::NodeAddedEvent, this, SLOT(onMarkupNodeAdded())); 
}


stkFiducialMarkerRegistrationWidget::~stkFiducialMarkerRegistrationWidget()
{

}

void stkFiducialMarkerRegistrationWidget::UseTrackerAurora(int comPort)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->TrackerType = TRACKER_TYPE_AURORA;
	stkAuroraTracker* tracker = new stkAuroraTracker;
	tracker->setComPortNum(comPort);
	d->Tracker = tracker;
}

void stkFiducialMarkerRegistrationWidget::UseTrackerPolaris(int comPort)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->TrackerType = TRACKER_TYPE_POLARIS;
	stkPolarisTracker* tracker = new stkPolarisTracker;
	tracker->setComPortNum(comPort);
	d->Tracker = tracker;
}

void stkFiducialMarkerRegistrationWidget::UseTrackerAscension()
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->TrackerType = TRACKER_TYPE_ASCENSION;

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

void stkFiducialMarkerRegistrationWidget::on_DeleteFiducialMarkerToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	// get the selected rows
	QList<QTableWidgetItem *> selectedItems = d->FiducialMarkerTableWidget->selectedItems();

	// first, check if nothing is selected
	if (selectedItems.isEmpty())
	{
		return;
	}

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	// get the active node
	vtkMRMLNode *mrmlNode = scene->GetNodeByID("vtkMRMLMarkupsFiducialNode1");
	if(!mrmlNode) return;
	vtkMRMLMarkupsNode *listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
	if (!listNode){
		return;
	}

	
	// iterate over the selected items and save their row numbers (there are
	// selected indices for each column in a row, so jump by the number of
	// columns), so can delete without relying on the table
	QList<int> rows;
	
	for (int i = 0; i < selectedItems.size(); i += d->FiducialMarkerTableWidget->columnCount())
	{
		// get the row
		int row = selectedItems.at(i)->row();
		// qDebug() << "Saving: i = " << i << ", row = " << row;
		rows << row;
	}
	// sort the list
	qSort(rows);

	ctkMessageBox deleteAllMsgBox;
	deleteAllMsgBox.setWindowTitle("Delete Fiducial Markers in this list?");
	QString labelText = QString("Delete ")
		+ QString::number(rows.size())
		+ QString(" Fiducial Markers from this list?");
	// don't show again check box conflicts with informative text, so use
	// a long text
	deleteAllMsgBox.setText(labelText);
	deleteAllMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	deleteAllMsgBox.setDefaultButton(QMessageBox::Yes);
	deleteAllMsgBox.setDontShowAgainVisible(true);
	deleteAllMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysDeleteMarkups");
	int ret = deleteAllMsgBox.exec();
	if (ret == QMessageBox::Yes)
	{
		// delete from the end
		for (int i = rows.size() - 1; i >= 0; --i)
		{
			int index = rows.at(i);
			// qDebug() << "Deleting: i = " << i << ", index = " << index;
			// remove the markup at that row
			listNode->RemoveMarkup(index);
		}
	}

	// clear the selection on the table
	d->FiducialMarkerTableWidget->clearSelection();
}

void stkFiducialMarkerRegistrationWidget::on_ClearFiducialMarkerToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	// get the active node
	vtkMRMLNode *mrmlNode = scene->GetNodeByID("vtkMRMLMarkupsFiducialNode1");
	if(!mrmlNode) return;
	vtkMRMLMarkupsNode *listNode = vtkMRMLMarkupsNode::SafeDownCast(mrmlNode);
	if (!listNode){
		return;
	}

	// qDebug() << "Removing markups from list " << listNode->GetName();
	ctkMessageBox deleteAllMsgBox;
	deleteAllMsgBox.setWindowTitle("Delete All Fiducial Markers in this list?");
	QString labelText = QString("Delete all ")
		+ QString::number(listNode->GetNumberOfMarkups())
		+ QString(" Markups in this list?");
	// don't show again check box conflicts with informative text, so use
	// a long text
	deleteAllMsgBox.setText(labelText);
	deleteAllMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	deleteAllMsgBox.setDefaultButton(QMessageBox::Yes);
	deleteAllMsgBox.setDontShowAgainVisible(true);
	deleteAllMsgBox.setDontShowAgainSettingsKey("Markups/AlwaysDeleteAllMarkups");
	int ret = deleteAllMsgBox.exec();
	if (ret == QMessageBox::Yes)
	{
		listNode->RemoveAllMarkups();
	}

}


void stkFiducialMarkerRegistrationWidget::onMarkupNodeAdded()
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

	observeMarkupsNode(markupsNode);

	updateFiducialMarkers();
}


//-----------------------------------------------------------------------------
void stkFiducialMarkerRegistrationWidget::observeMarkupsNode(vtkMRMLNode *markupsNode)
{
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	
	// remove all connections
	vtkCollection *col = scene->GetNodesByClass("vtkMRMLMarkupsNode");
	unsigned int numNodes = col->GetNumberOfItems();
	for (unsigned int n = 0; n < numNodes; n++)
	{
		vtkMRMLNode *node = vtkMRMLNode::SafeDownCast(col->GetItemAsObject(n));
		if (node)
		{
			if (markupsNode)
			{
				// is this the markups node?
				if (node->GetID() && markupsNode->GetID() && strcmp(node->GetID(), markupsNode->GetID()) == 0)
				{
					// don't disconnect
					// qDebug() << "\tskipping disconnecting " << node->GetID();
					continue;
				}
			}
			// qDebug() << "\tdisconnecting " << node->GetID();
			//this->qvtkDisconnect(node, vtkMRMLMarkupsNode::LockModifiedEvent,					this, SLOT(onActiveMarkupsNodeLockModifiedEvent()));
			//this->qvtkDisconnect(node, vtkMRMLMarkupsNode::LabelFormatModifiedEvent,			this, SLOT(onActiveMarkupsNodeLabelFormatModifiedEvent()));
			this->qvtkDisconnect(node, vtkMRMLMarkupsNode::PointModifiedEvent,					this, SLOT(onActiveMarkupsNodePointModifiedEvent(vtkObject*,vtkObject*)));
			//this->qvtkDisconnect(node, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,			this, SLOT(onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject*,vtkObject*)));
			this->qvtkDisconnect(node, vtkMRMLMarkupsNode::MarkupAddedEvent,					this, SLOT(onActiveMarkupsNodeMarkupAddedEvent()));
			this->qvtkDisconnect(node, vtkMRMLMarkupsNode::MarkupRemovedEvent,					this, SLOT(onActiveMarkupsNodeMarkupRemovedEvent()));

			// display node
			vtkMRMLMarkupsNode *displayableNode = vtkMRMLMarkupsNode::SafeDownCast(node);
			if (displayableNode)
			{
				vtkMRMLDisplayNode *displayNode = displayableNode->GetDisplayNode();
				if (displayNode)
				{
					this->qvtkDisconnect(displayNode, vtkCommand::ModifiedEvent,				this, SLOT(onActiveMarkupsNodeDisplayModifiedEvent()));
				}
			}
		}
	}
	col->RemoveAllItems();
	col->Delete();


	if (markupsNode)
	{
		// is the node already connected?
		if (this->qvtkIsConnected(markupsNode, vtkMRMLMarkupsNode::LockModifiedEvent,		this, SLOT(onActiveMarkupsNodeLockModifiedEvent())))
		{
			// qDebug() << "\tmarkups node is already connected: " << markupsNode->GetID();
		}
		else
		{
			// add connections for this node
			//this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::LockModifiedEvent,				this, SLOT(onActiveMarkupsNodeLockModifiedEvent()));
			//this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::LabelFormatModifiedEvent,		this, SLOT(onActiveMarkupsNodeLabelFormatModifiedEvent()));
			this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::PointModifiedEvent,				this, SLOT(onActiveMarkupsNodePointModifiedEvent(vtkObject*,vtkObject*)));
			//this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::NthMarkupModifiedEvent,		this, SLOT(onActiveMarkupsNodeNthMarkupModifiedEvent(vtkObject*,vtkObject*)));
			this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::MarkupAddedEvent,				this, SLOT(onActiveMarkupsNodeMarkupAddedEvent()));
			this->qvtkConnect(markupsNode, vtkMRMLMarkupsNode::MarkupRemovedEvent,				this, SLOT(onActiveMarkupsNodeMarkupRemovedEvent()));
			// qDebug() << "\tconnected markups node " << markupsNode->GetID();
			// display node
			vtkMRMLMarkupsNode *displayableNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNode);
			if (displayableNode)
			{
				vtkMRMLDisplayNode *displayNode = displayableNode->GetDisplayNode();
				if (displayNode)
				{
					this->qvtkConnect(displayNode, vtkCommand::ModifiedEvent,this, SLOT(onActiveMarkupsNodeDisplayModifiedEvent()));
				}
			}
		}
	}
}



//-----------------------------------------------------------------------------
void stkFiducialMarkerRegistrationWidget::onActiveMarkupsNodeMarkupAddedEvent()//vtkMRMLNode *markupsNode)
{
	updateFiducialMarkers();
}

void stkFiducialMarkerRegistrationWidget::onActiveMarkupsNodePointModifiedEvent(vtkObject *caller, vtkObject *callData)
{
	updateFiducialMarkers();
}

void stkFiducialMarkerRegistrationWidget::onActiveMarkupsNodeMarkupRemovedEvent()
{
	updateFiducialMarkers();
}

void stkFiducialMarkerRegistrationWidget::onActiveMarkupsNodeDisplayModifiedEvent()
{
	updateFiducialMarkers();
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



void stkFiducialMarkerRegistrationWidget::on_StartTrackingToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	if(d->StartTrackingToolButton->isChecked())
		this->StartTracking();
	else
		this->StopTracking();
}


bool stkFiducialMarkerRegistrationWidget::StartTracking()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	if(d->TrackerType == TRACKER_TYPE_AURORA)
	{
		if(!d->Tracker->Open())
			return false;

		d->CalibrationTool = d->Tracker->AttachTrackerTool("CalibrationTool", "0");
		if(!d->CalibrationTool)
			return false;

		connect(d->CalibrationTool, SIGNAL(dataValidChanged(bool)),this,SLOT(setCalibrationToolDataValid(bool)));
	}


	return true;
}

void stkFiducialMarkerRegistrationWidget::StopTracking()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

}

void stkFiducialMarkerRegistrationWidget::setCalibrationToolDataValid(bool valid)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->CalibrationToolButton->setEnabled(valid);
}


