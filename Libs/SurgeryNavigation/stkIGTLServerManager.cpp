
#include "stkIGTLServerManager.h"
#include "qMRMLNodeFactory.h"
#include "stkMRMLHelper.h"
#include "stkMRMLIGTLServerNode.h"
#include "stkIGTLToMRMLPosition.h"
#include "stkIGTLToMRMLImage.h"
#include "vtkNew.h"

class stkIGTLServerManagerPrivate
{
public:
	stkMRMLIGTLServerNode*	IGTLServerNode;
	stkMRMLIGTLServerNode*	IGTLImageServerNode;
	vtkSmartPointer<stkIGTLToMRMLPosition> PositionConverter;
	vtkSmartPointer<stkIGTLToMRMLImage> ImageConverter;
};


stkIGTLServerManager::stkIGTLServerManager(QObject *parent)
	: QObject(parent),d_ptr(new stkIGTLServerManagerPrivate)
{
	Q_D(stkIGTLServerManager);
	d->IGTLServerNode = NULL;
	d->IGTLImageServerNode = NULL;
	d->PositionConverter = NULL;
	d->ImageConverter = NULL;

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;
	
	scene->RegisterNodeClass(vtkNew<stkMRMLIGTLServerNode>().GetPointer());
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


void stkIGTLServerManager::StartImageServer()
{
	Q_D(stkIGTLServerManager);

	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if (!scene)	return;

	if(!d->IGTLImageServerNode)
	{
		vtkMRMLNode * node = qMRMLNodeFactory::createNode(scene, "stkMRMLIGTLServerNode");
		d->IGTLImageServerNode = stkMRMLIGTLServerNode::SafeDownCast(node);
		d->IGTLImageServerNode->DisableModifiedEventOn();
		d->IGTLImageServerNode->SetServerPort(18945);//RTImageServerÊ¹ÓÃ18945¶Ë¿Ú
		d->IGTLImageServerNode->SetName("IGTLImageServer");
		d->IGTLImageServerNode->DisableModifiedEventOff();
		d->IGTLImageServerNode->InvokePendingModifiedEvent();

		d->ImageConverter = vtkSmartPointer<stkIGTLToMRMLImage>::New();
		d->IGTLImageServerNode->RegisterMessageConverter(d->ImageConverter);
	}

	if( d->IGTLImageServerNode && d->IGTLImageServerNode->GetState() == stkMRMLIGTLServerNode::STATE_OFF )
	{
		d->IGTLImageServerNode->Start();
		d->IGTLImageServerNode->Modified();
	}	
}


void stkIGTLServerManager::StopImageServer()
{
	Q_D(stkIGTLServerManager);

	if(!d->IGTLImageServerNode)
		return;

	if ( d->IGTLImageServerNode->GetState() != stkMRMLIGTLServerNode::STATE_OFF )
	{
		d->IGTLImageServerNode->Stop();
	}

}

void stkIGTLServerManager::importDataAndEvents()
{
	Q_D(stkIGTLServerManager);
	
	if(d->IGTLServerNode)	d->IGTLServerNode->ImportEventsFromEventBuffer();
	if(d->IGTLImageServerNode)	d->IGTLImageServerNode->ImportEventsFromEventBuffer();
	if(d->IGTLServerNode) d->IGTLServerNode->ImportDataFromCircularBuffer();
	if(d->IGTLImageServerNode) d->IGTLImageServerNode->ImportDataFromCircularBuffer();
}

