#include "stkFiducialMarkerRegistrationWidget.h"
#include "ui_stkFiducialMarkerRegistrationWidget.h"

#include <QTimer>

#include <ctkVTKObject.h>
#include "ctkMessageBox.h"

#include "qSlicerApplication.h"

#include "vtkCollection.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLNode.h"
#include "qMRMLNodeFactory.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include <vtkNew.h>

// ITK includes
#include "itkPoint.h"
#include <itkAffineTransform.h>
#include <itkImage.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <itkSimilarity3DTransform.h>
#include <itkTransformFileWriter.h>

// STD includes
#include <numeric>

#include "stkMRMLHelper.h"
#include <vtkMatrix4x4.h>
#include "stkTrackerTool.h"
#include <vtkSmartPointer.h>
#include <QThread>

typedef  std::vector<itk::Point<double, 3> > PointList;

class stkFiducialMarkerRegistrationWidgetPrivate : public Ui_stkFiducialMarkerRegistrationWidget
{
public:
	PointList FiducialPoints;
	PointList ToolPoints;
	std::vector<bool> ToolPointsCollected;

	bool ComputeRegistrationTransform(vtkMRMLLinearTransformNode* tnode);

	vtkMRMLLinearTransformNode* IGTTransformNode;
};


stkFiducialMarkerRegistrationWidget::stkFiducialMarkerRegistrationWidget(QWidget *parent)
	: Superclass(parent),d_ptr(new stkFiducialMarkerRegistrationWidgetPrivate)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->setupUi(this);

	
	d->IGTTransformNode = NULL;
	d->FiducialMarkerTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->FiducialMarkerTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); 
	d->FiducialMarkerTableWidget->setColumnWidth(0,30);
	d->FiducialMarkerTableWidget->setColumnWidth(1,80);
	d->FiducialMarkerTableWidget->setColumnWidth(2,80);

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	this->qvtkConnect(scene,vtkMRMLScene::NodeAddedEvent, this, SLOT(onMarkupNodeAdded())); 

	d->IGTTransformNode = vtkMRMLLinearTransformNode::New();	
	d->IGTTransformNode->SetName("IGTTransform");
	d->IGTTransformNode->SetDescription("Tracker Transform");
	scene->AddNode(d->IGTTransformNode);
	d->IGTTransformNode->Delete();
}


stkFiducialMarkerRegistrationWidget::~stkFiducialMarkerRegistrationWidget()
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->IGTTransformNode = NULL;
}

void stkFiducialMarkerRegistrationWidget::on_AddFiducialMarkerToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
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

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
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

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
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
	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
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

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	vtkMRMLNode *markupsNodeMRML = scene->GetNodeByID("vtkMRMLMarkupsFiducialNode1");
	if(!markupsNodeMRML) return;

	vtkMRMLMarkupsNode *markupsNode = vtkMRMLMarkupsNode::SafeDownCast(markupsNodeMRML);
	if(!markupsNode) return;

	d->FiducialMarkerTableWidget->clearContents();

	int NumberOfMarkups = markupsNode->GetNumberOfMarkups();
	if ( NumberOfMarkups<= 0)
		return;

	d->FiducialMarkerTableWidget->setRowCount(NumberOfMarkups);
	if( d->FiducialPoints.size() != NumberOfMarkups)
	{
		d->FiducialPoints.resize(NumberOfMarkups);
		d->ToolPoints.resize(NumberOfMarkups);
		d->ToolPointsCollected.resize(NumberOfMarkups);
		for (int i = 0 ; i < NumberOfMarkups; i++)
		{
			d->ToolPointsCollected[i] = false;
		}
	}

	for (int i = 0; i < NumberOfMarkups; i++)
	{		
		double coordinate[3];
		markupsNode->GetMarkupPoint(i, 0, coordinate);

		QString markupLabel = QString(markupsNode->GetNthMarkupLabel(i).c_str());
		d->FiducialMarkerTableWidget->setItem(i,0,new QTableWidgetItem(markupLabel));		

		itk::Point<double, 3> point;
		point[0] = coordinate[0];
		point[1] = coordinate[1];
		point[2] = coordinate[2];
		d->FiducialPoints[i]= point;		

		char str[50];
		sprintf(str, "%.0f,%.0f,%.0f",point[0],point[1],point[2]);
		d->FiducialMarkerTableWidget->setItem(i,1,new QTableWidgetItem(str));		

		if (d->ToolPointsCollected[i])
		{
			point = d->ToolPoints[i];
			sprintf(str, "%.0f,%.0f,%.0f",point[0],point[1],point[2]);
			d->FiducialMarkerTableWidget->setItem(i,2,new QTableWidgetItem(str));		
		}
		else{
			d->FiducialMarkerTableWidget->setItem(i,2,new QTableWidgetItem("NA"));		
		}

	}
	d->FiducialMarkerTableWidget->selectRow(0);
}

void stkFiducialMarkerRegistrationWidget::on_CalibrationToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	double x=0.0, y=0.0, z=0.0;

	vtkMRMLLinearTransformNode *calibTransform = vtkMRMLLinearTransformNode::SafeDownCast(stkMRMLHelper::GetSingleMRMLNodeByName("CalibrationTool"));
	if (calibTransform == NULL)
		return;

	x = calibTransform->GetMatrixTransformToParent()->GetElement(0, 3);
	y = calibTransform->GetMatrixTransformToParent()->GetElement(1, 3);
	z = calibTransform->GetMatrixTransformToParent()->GetElement(2, 3);

	int row = d->FiducialMarkerTableWidget->currentRow();
	if(row<0)
		return;

	char str[50];
	sprintf(str, "%.0f,%.0f,%.0f",x,y,z);
	d->FiducialMarkerTableWidget->setItem(row,2,new QTableWidgetItem(str));		

	itk::Point<double, 3> point;
	point[0] = x;
	point[1] = y;
	point[2] = z;
	d->ToolPoints[row]= point;	
	d->ToolPointsCollected[row] = true;

	//automatic jump to the next row
	if((row+1) < d->FiducialMarkerTableWidget->rowCount()){
		row++;
		d->FiducialMarkerTableWidget->selectRow(row);
	}else{
		d->FiducialMarkerTableWidget->selectRow(0);
	}	

	d->ComputeRegistrationTransform(d->IGTTransformNode);

	calibTransform->SetAndObserveTransformNodeID(d->IGTTransformNode->GetID());
}

void stkFiducialMarkerRegistrationWidget::setCalibrationToolDataValid(bool valid)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->CalibrationToolButton->setEnabled(valid);
}

bool stkFiducialMarkerRegistrationWidgetPrivate::ComputeRegistrationTransform(vtkMRMLLinearTransformNode* tnode)
{
	int NumberOfMarkups = FiducialPoints.size();
	if (NumberOfMarkups < 4)
		return false;

	//if there is some point not collected, return
	for (int i = 0; i<NumberOfMarkups;i++)
	{
		if(!ToolPointsCollected[i])
			return false;
	}

	//计算ITK Transform
	typedef itk::Similarity3DTransform<double> SimilarityTransformType;
	SimilarityTransformType::Pointer  simiTrans = SimilarityTransformType::New();
	simiTrans->SetIdentity();
	simiTrans->SetScale(1.0);

	typedef itk::LandmarkBasedTransformInitializer<SimilarityTransformType,
		itk::Image<short, 3>, itk::Image<short, 3> > InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();

	//这里需要临时将fixedPoints和movingPoints各点的坐标的xy值取反，z值不变，暂时没有弄清原因
	PointList fixedPoints;
	PointList movingPoints;
	fixedPoints.resize(NumberOfMarkups);
	movingPoints.resize(NumberOfMarkups);
	for( int i = 0 ; i < NumberOfMarkups; i++)
	{
		fixedPoints[i][0] = -FiducialPoints[i][0];
		fixedPoints[i][1] = -FiducialPoints[i][1];
		fixedPoints[i][2] = FiducialPoints[i][2];

		movingPoints[i][0] = -ToolPoints[i][0];
		movingPoints[i][1] = -ToolPoints[i][1];
		movingPoints[i][2] = ToolPoints[i][2];
	}

	initializer->SetTransform(simiTrans);
	initializer->SetFixedLandmarks(fixedPoints);
	initializer->SetMovingLandmarks(movingPoints);
	initializer->InitializeTransform();

	itk::AffineTransform<double, 3>::Pointer transform = itk::AffineTransform<double, 3>::New();
	transform->SetCenter(simiTrans->GetCenter() );
	transform->SetMatrix(simiTrans->GetMatrix() );
	transform->SetTranslation(simiTrans->GetTranslation() );

	int i, j;
	vtkSmartPointer<vtkMatrix4x4> vtkmat = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkmat->Identity();

	typedef itk::MatrixOffsetTransformBase<double,3,3> DoubleLinearTransformType;// Linear transform of doubles, dimension 3
	DoubleLinearTransformType::Pointer dlt = dynamic_cast<DoubleLinearTransformType*>( transform.GetPointer() );
	if (dlt)
	{
		for (i=0; i < 3; i++)
		{
			for (j=0; j < 3; j++)
			{
				(*vtkmat)[i][j] = dlt->GetMatrix()[i][j];
			}
			(*vtkmat)[i][3] = dlt->GetOffset()[i];
		}
	}
	else{
		return false;
	}


	vtkSmartPointer<vtkMatrix4x4> lps2ras = vtkSmartPointer<vtkMatrix4x4>::New();
	lps2ras->Identity();
	(*lps2ras)[0][0] = (*lps2ras)[1][1] = -1.0;

	vtkSmartPointer<vtkMatrix4x4> ras2lps = vtkSmartPointer<vtkMatrix4x4>::New();
	ras2lps->Identity();
	(*ras2lps)[0][0] = (*ras2lps)[1][1] = -1.0;			

	// Convert from LPS (ITK) to RAS (Slicer)
	//
	// Tras = lps2ras * Tlps * ras2lps
	//
	vtkMatrix4x4::Multiply4x4(lps2ras, vtkmat, vtkmat);
	vtkMatrix4x4::Multiply4x4(vtkmat, ras2lps, vtkmat);

	// Convert the sense of the transform (from an ITK resampling
	// transform to a Slicer modeling transform)
	//
	vtkmat->Invert();

	// Set the matrix on the node
	tnode->SetAndObserveMatrixTransformToParent( vtkmat );

	return true;
}

