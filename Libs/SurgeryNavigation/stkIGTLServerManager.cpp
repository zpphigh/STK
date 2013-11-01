
#include "stkIGTLServerManager.h"
#include "qMRMLNodeFactory.h"
#include "stkMRMLHelper.h"
#include "stkMRMLIGTLServerNode.h"
#include "stkIGTLToMRMLPosition.h"
#include "vtkNew.h"

class stkIGTLServerManagerPrivate
{
public:
	stkMRMLIGTLServerNode*	IGTLServerNode;
	vtkSmartPointer<stkIGTLToMRMLPosition> PositionConverter;
};


stkIGTLServerManager::stkIGTLServerManager(QObject *parent)
	: QObject(parent),d_ptr(new stkIGTLServerManagerPrivate)
{
	Q_D(stkIGTLServerManager);
	d->IGTLServerNode = NULL;
	d->PositionConverter = NULL;
}

stkIGTLServerManager::~stkIGTLServerManager()
{
	Q_D(stkIGTLServerManager);

}

void stkIGTLServerManager::StartIGTLServer()
{
	Q_D(stkIGTLServerManager);

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	if(!d->IGTLServerNode)
	{
		scene->RegisterNodeClass(vtkNew<stkMRMLIGTLServerNode>().GetPointer());

		vtkMRMLNode * node = qMRMLNodeFactory::createNode(scene, "stkMRMLIGTLServerNode");
		d->IGTLServerNode = stkMRMLIGTLServerNode::SafeDownCast(node);
		d->IGTLServerNode->DisableModifiedEventOn();
		d->IGTLServerNode->SetServerPort(18944); //TrackServer use port 18944
		d->IGTLServerNode->SetName("IGTLServer");
		d->IGTLServerNode->DisableModifiedEventOff();
		d->IGTLServerNode->InvokePendingModifiedEvent();
		d->PositionConverter = vtkSmartPointer<stkIGTLToMRMLPosition>::New();
		d->IGTLServerNode->RegisterMessageConverter(d->PositionConverter);
	}

	if( d->IGTLServerNode && d->IGTLServerNode->GetState() == stkMRMLIGTLServerNode::STATE_OFF )
	{
		d->IGTLServerNode->Start();
		d->IGTLServerNode->Modified();
	}	
}

void stkIGTLServerManager::StopIGTServer()
{
	Q_D(stkIGTLServerManager);

	if(!d->IGTLServerNode )
		return;

	if ( d->IGTLServerNode->GetState() != stkMRMLIGTLServerNode::STATE_OFF )
	{
		d->IGTLServerNode->Stop();
	}
}

void stkIGTLServerManager::importDataAndEvents()
{
	Q_D(stkIGTLServerManager);
	
	d->IGTLServerNode->ImportEventsFromEventBuffer();
	d->IGTLServerNode->ImportDataFromCircularBuffer();
}

