

// Qt includes
//#include <QDebug>
#include <QtGui/QWidget>
#include <QList>
#include <QTableWidgetSelectionRange>

// SlicerQt includes
#include "stkRoutePlanningWidget.h"
#include "ui_stkRoutePlanningWidget.h"

// MRML includes
#include "qSlicerApplication.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "stkMRMLProbeNode.h"
#include "vtkMRMLCrossPointNode.h"
#include "vtkMRMLVolumeNode.h"

#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class stkRoutePlanningWidgetPrivate: public Ui_stkRoutePlanningWidget
{
public:

};
 

//-----------------------------------------------------------------------------
// stkRoutePlanningWidget methods

//-----------------------------------------------------------------------------
stkRoutePlanningWidget::stkRoutePlanningWidget(QWidget* _parent)
  : Superclass( _parent ),d_ptr( new stkRoutePlanningWidgetPrivate )
{
	Q_D(stkRoutePlanningWidget);
  d->setupUi(this);

  this->SetCurrentProbeNode("");

  d->addProbeButton->setCheckable(true);
  d->addProbeButton->setChecked(false);

  d->probeTable->setWindowTitle("Probe");
  d->probeTable->setColumnCount(4);
  QStringList header; 
  header.append(QString::fromLocal8Bit("ID"));
  header.append(QString::fromLocal8Bit("P"));
  header.append(QString::fromLocal8Bit("R"));
  //header.append(QString::fromLocal8Bit("ID"));

  d->probeTable->setHorizontalHeaderLabels(header); 
  d->probeTable->setColumnWidth(0,30);
  d->probeTable->setColumnWidth(1,85);
  d->probeTable->setColumnWidth(2,85);

  d->probeTable->hideColumn(3); // 第五列保存node的ID，程序界面不显示;

  d->probeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  d->probeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->probeTable->setSelectionMode(QAbstractItemView::SingleSelection);

  this->connect(d->probeTable, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onProbeTableRowSelected(const QModelIndex&)));

  QHeaderView* headerView = d->probeTable->verticalHeader();//隐藏左侧序号
  headerView->setHidden(true);

  //--------------------------------------------------------------------------------
  // setup的同时，将interaction node与selection node的状态与窗口中的过程联系起来;
  // 如果发生交互状态改变或者selection node改变的时候，及时改变widget中button的状态;
  //--------------------------------------------------------------------------------
  vtkMRMLInteractionNode* interationNode = vtkMRMLInteractionNode::SafeDownCast(
	  qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));

  if (interationNode)
  {
      // 将qt的widget与vtk的event事件联系起来;
      this->qvtkConnect(interationNode, vtkMRMLInteractionNode::InteractionModeChangedEvent,
		  this, SLOT(updateWidgetFromMRML()));
	  this->qvtkConnect(interationNode, vtkMRMLInteractionNode::InteractionModePersistenceChangedEvent,
		  this, SLOT(updateWidgetFromMRML()));
  }

  vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(
	  qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));

  if (selectionNode)
  {
	  const char* aName = selectionNode->GetActivePlaceNodeID();

	  // 将qt的widget与vtk的event事件联系起来;
      this->qvtkConnect(selectionNode, vtkMRMLSelectionNode::ActivePlaceNodeIDChangedEvent,
	  this, SLOT(onActivePlaceNodeIDChanged()));
	  this->qvtkConnect(selectionNode, vtkMRMLSelectionNode::ActivePlaceNodeClassNameChangedEvent,
		  this, SLOT(onActivePlaceNodeNameChanged(aName)));
  }

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
stkRoutePlanningWidget::~stkRoutePlanningWidget()
{
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::on_addProbeButton_clicked()
{
	Q_D(stkRoutePlanningWidget);

	vtkMRMLCrossPointNode* cNode = qSlicerCoreApplication::application()->mrmlScene()?
		vtkMRMLCrossPointNode::SafeDownCast(qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLCrossPointNode")):NULL;
	double cross[4] = {0, 0, 0, 1};
	cNode->GetCrossPointCoordinates(cross);

	this->qvtkConnect(qSlicerCoreApplication::application()->mrmlScene(),vtkMRMLScene::NodeAddedEvent, 
		this, SLOT(onNodeAdded(vtkObject*, vtkObject*)));

	vtkMRMLInteractionNode* interationNode = vtkMRMLInteractionNode::SafeDownCast(
		qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));

	if (!interationNode)
	{
		return;
	}

	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(
		qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));

	if (!selectionNode)
	{
		return;
	}

	// 
	if (d->addProbeButton->isChecked())
	{
		selectionNode->SetActivePlaceNodeClassName("vtkMRMLProbeNode");

		// 一次只添加一跟针;
		interationNode->SwitchToSinglePlaceMode(); // 点击按钮可以直接加针
	}
	else
	{
		selectionNode->SetActivePlaceNodeID("");
		interationNode->SwitchToViewTransformMode();
	}
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::updateWidgetFromMRML()
{
	 Q_D(stkRoutePlanningWidget);

	vtkMRMLInteractionNode* interationNode = vtkMRMLInteractionNode::SafeDownCast(
		qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));

	if (!interationNode)
	{
		return;
	}

	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(
		qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));

	if (!selectionNode)
	{
		return;
	}
	
	const char* aName = selectionNode->GetActivePlaceNodeID();;

	// 根据interaction node的当前状态，调用updateWidgetFromAnnotationID()，改变button的状态;
	int currentMode = interationNode->GetCurrentInteractionMode();
	switch (currentMode)
	{
	case vtkMRMLInteractionNode::Place:
		this->onActivePlaceNodeNameChanged(aName);
		break;
	case vtkMRMLInteractionNode::ViewTransform:
		this->onActivePlaceNodeNameChanged(0);
		break;
	default:
		break;
    }
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::onActivePlaceNodeNameChanged(const char* id)
{
	Q_D(stkRoutePlanningWidget);

	if (!id)
	{
		d->addProbeButton->setChecked(false);
	}

	// 将传入的annotation ID转换为QString的类型，方便判断annotation的类名;
	const QString aName = id;
	// 根据相应的annotation的名字，改变相应的button的checked状态;
	if (aName.compare("vtkMRMLProbeNode") == 0)
	{
		d->addProbeButton->setChecked(true);
	}
	else
	{
		d->addProbeButton->setChecked(false);
	}
}
//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::onActivePlaceNodeIDChanged()
{
	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(
		qSlicerCoreApplication::application()->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));

	if (!selectionNode)
	{
		return;
	}

	char* active = selectionNode->GetActivePlaceNodeID();
	std::string Name = std::string(active);

	std::string currentNode = this->GetCurrentProbeNode();
	if (currentNode != Name)
	{
		this->SetCurrentProbeNode(Name);
	}
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::on_deleteProbeButton_Clicked()
{
	Q_D(stkRoutePlanningWidget);

	std::string probeID = this->GetCurrentProbeNode();
	if (probeID.empty())
	{
		return;
	}

	vtkMRMLProbeNode* probe = vtkMRMLProbeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(probeID));
	if (!probe)
	{
		return;
	}
	this->mrmlScene()->RemoveNode(probe);

	// 最好遍历probeTable的第五列或者所有item，找到当前node的ID所在的行，然后在删除;
	int rows = d->probeTable->rowCount();
	int row = 0;
	for (int i = 0; i < rows; i++)
	{
		QTableWidgetItem* item = d->probeTable->item(i, 3);
		QString str = item->text();
		if (!str.isEmpty())
		{
			std::string ID = str.toStdString();
			const char* id = ID.data();
			const char* probe = probeID.data();
			if (!strcmp(id, probe))
			{
				row = item->row();
			}
		}
	}

	d->probeTable->removeRow(row);

	QTableWidgetItem* item = d->probeTable->item(row, 3);
	if (item)
	{
		QString id = item->text();
		std::string ID = id.toStdString();
		this->SetCurrentProbeNode(ID);
	}

	this->UpdateProbeTabelFromMRML();
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::on_clearProbesButton_Clicked()
{
	Q_D(stkRoutePlanningWidget);

	int rows = d->probeTable->rowCount();
	for (int i = 0; i < rows; i++)
	{
		QTableWidgetItem* item = d->probeTable->item(i, 3);
		QString str = item->text();
		if (!str.isEmpty())
		{
			std::string id = str.toStdString();

			vtkMRMLProbeNode* probe = vtkMRMLProbeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(id));
			if (probe)
			{
				this->mrmlScene()->RemoveNode(probe);
			}
		}
	}

	for (int i = 0; i < rows; i++)
	{
		d->probeTable->removeRow(0);
	}
}


//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::onNodeAdded(vtkObject* mrmlScene, vtkObject* mrmlNode)
{
	Q_UNUSED(mrmlScene);

	if (mrmlNode)
	{
		vtkMRMLProbeNode* probe = vtkMRMLProbeNode::SafeDownCast(mrmlNode);

		if (!probe)
		{
			return;
		}

		std::string currentProbeID = std::string(probe->GetID());
		std::string currentNode = this->GetCurrentProbeNode();
		if (currentNode != currentProbeID)
		{
			this->SetCurrentProbeNode(currentProbeID);
		}
	}

	this->AddRow();

	this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::AddRow()
{
	Q_D(stkRoutePlanningWidget);

	std::string probeID = this->GetCurrentProbeNode();
	if (probeID.empty())
	{
		return;
	}
	QString ID = QString::fromLocal8Bit(probeID.c_str());

	vtkMRMLProbeNode* probe = vtkMRMLProbeNode::SafeDownCast(qSlicerCoreApplication::application()->mrmlScene()->GetNodeByID(probeID));
	char strHeader[50], strTail[50];
	double valHeader[4]={0,0,0,1}, valTail[4]={0,0,0,1};
	probe->GetHeader(valHeader);
	probe->GetTail(valTail);
	sprintf(strHeader,"%.1f,%.1f,%.1f",valHeader[0],valHeader[1],valHeader[2]);
	sprintf(strTail,"%.1f,%.1f,%.1f",valTail[0],valTail[1],valTail[2]);
	// 动态添加probe Table的内容;
	d->probeTable->setRowCount(d->probeTable->rowCount() + 1);

	d->probeTable->setItem(d->probeTable->rowCount() - 1, 0, new QTableWidgetItem(QString::number(d->probeTable->rowCount())));
	d->probeTable->item(d->probeTable->rowCount() - 1, 0)->setTextAlignment(Qt::AlignCenter);
	d->probeTable->setItem(d->probeTable->rowCount() - 1, 1, new QTableWidgetItem(strHeader));
	d->probeTable->item(d->probeTable->rowCount() - 1, 1)->setTextAlignment(Qt::AlignCenter);
	d->probeTable->setItem(d->probeTable->rowCount() - 1, 2, new QTableWidgetItem(strTail));
	d->probeTable->item(d->probeTable->rowCount() - 1, 2)->setTextAlignment(Qt::AlignCenter);
	d->probeTable->setItem(d->probeTable->rowCount() - 1, 3, new QTableWidgetItem(ID));          // 保存当前node的ID;
}
//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::UpdateProbeTabelFromMRML()
{
	Q_D(stkRoutePlanningWidget);

	std::string probeID = this->GetCurrentProbeNode();
	vtkMRMLProbeNode* probe = vtkMRMLProbeNode::SafeDownCast(qSlicerCoreApplication::application()->mrmlScene()->GetNodeByID(probeID));
	if (!probe)
	{
		return;
	}

	int rows = d->probeTable->rowCount();
	int index = 0;
	for (int i = 0; i<rows; i++)
	{
		QTableWidgetItem* item = d->probeTable->item(i,0);
		int row = item->row() + 1;
		item->setText(QString::number(row));

		QTableWidgetItem* item2 = d->probeTable->item(i,3);
		QString str = item2->text();
		std::string ID = str.toStdString();
		const char* pID = ID.data();

		const char* probe = probeID.data();
		if (!strcmp(pID, probe))
		{
			index = item2->row();
		}
	}

	char strHeader[50], strTail[50];
	double valHeader[4]={0,0,0,1}, valTail[4]={0,0,0,1};
	probe->GetHeader(valHeader);
	probe->GetTail(valTail);
	sprintf(strHeader,"%.1f,%.1f,%.1f",valHeader[0],valHeader[1],valHeader[2]);
	sprintf(strTail,"%.1f,%.1f,%.1f",valTail[0],valTail[1],valTail[2]);

	d->probeTable->item(index, 1)->setText(strHeader);
	d->probeTable->item(index, 2)->setText(strTail);
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::onProbeTableRowSelected(const QModelIndex& index)
{
	Q_D(stkRoutePlanningWidget);

	// 恢复出选中的probe ID;
	int row = index.row();
	QString probeID = d->probeTable->item(row, 3)->text();
	std::string ID = probeID.toStdString();

	vtkMRMLScene* scene = qSlicerCoreApplication::application()->mrmlScene();
	if (!scene) return;

	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
	if (!selectionNode)  return;
	
	selectionNode->SetActivePlaceNodeID(ID.data());

	// 设置当前probe;
	this->SetCurrentProbeNode(ID);
}

//-----------------------------------------------------------------------------
void stkRoutePlanningWidget::SetCurrentProbeNode(std::string probe)
{
	Q_D(stkRoutePlanningWidget);

	if (probe.empty())
	{
		this->CurrentProbeNode = "";
		return;
	}

	/*int row = 0;

	QList<QTableWidgetItem *> currentProbe = d->probeTable->findItems(QString::fromLocal8Bit(probe.c_str()),Qt::MatchFixedString);
	for (int i = 0; i != currentProbe.size(); i++)
	{
		QTableWidgetItem* item = currentProbe.at(i);
		if (item->text().toStdString() == probe)
		{
			row = item->row();
		}
	}

	QTableWidgetSelectionRange selectionRange = QTableWidgetSelectionRange::QTableWidgetSelectionRange(row, 0, row, d->probeTable->columnCount());
	d->probeTable->setRangeSelected(selectionRange, true);*/

	vtkMRMLScene* scene = qSlicerCoreApplication::application()->mrmlScene();
	if (!scene) return;

	vtkMRMLProbeNode* preCurrentNode = NULL;
	std::string preCurrentProbe = this->GetCurrentProbeNode();
	if (preCurrentProbe != "")
	{
		preCurrentNode = vtkMRMLProbeNode::SafeDownCast(scene->GetNodeByID(preCurrentProbe.data()));
	}

	if (preCurrentNode)
	{
		preCurrentNode->SetCurrent(false);
	}
	
	vtkMRMLProbeNode* probeNode = vtkMRMLProbeNode::SafeDownCast(scene->GetNodeByID(probe.data()));
	if (!probeNode) return;

	std::string Name = std::string(probeNode->GetName());
	if (Name != "NavigationTool")
	{
		this->CurrentProbeNode = probe;
		probeNode->SetCurrent(true);
		probeNode->SetColor(188.0/255.0, 8.0/255.0, 8.0/255.0);
	}
}
