#include "stkMRMLHelper.h"
#include "qSlicerApplication.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkCollection.h"

vtkMRMLScene* stkMRMLHelper::mrmlScene()
{
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return NULL;

	return app->mrmlScene();
}


vtkMRMLNode* stkMRMLHelper::GetSingleMRMLNodeByName(char* name)
{
	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if(!scene) return NULL;

	vtkCollection *listCollection = scene->GetNodesByName(name);
	if (listCollection->GetNumberOfItems() != 1)
		return NULL; 

	listCollection->InitTraversal();
	vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(listCollection->GetNextItemAsObject());
	return node;
}