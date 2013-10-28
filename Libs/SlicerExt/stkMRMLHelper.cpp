#include "stkMRMLHelper.h"
#include "qSlicerApplication.h"
#include "vtkMRMLScene.h"

stkMRMLHelper::stkMRMLHelper()
{

}

stkMRMLHelper::~stkMRMLHelper()
{

}

vtkMRMLScene* stkMRMLHelper::mrmlScene()
{
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app)
		return NULL;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)
		return NULL;
}


