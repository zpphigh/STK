#include "stkSlicerDisplayHelper.h"

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include "stkMRMLHelper.h"

#include <vtkRenderer.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"

// --------------------------------------------------------------------------
void stkSlicerDisplayHelper::ResetFocalPoint()
{
	qSlicerApplication * app = qSlicerApplication::application();
	if (!app)	return;

	qSlicerLayoutManager * layoutManager = app->layoutManager();
	if (!layoutManager)	return;

	int threeDViewNum = layoutManager->threeDViewCount();
	for (int i=0; i<threeDViewNum; i++)
	{
		qMRMLThreeDWidget* qThreeDWidget = layoutManager->threeDWidget(i);
		if(qThreeDWidget)
		{
			qMRMLThreeDView* qTreeDView = qThreeDWidget->threeDView();
			if (qTreeDView)
				qTreeDView->resetFocalPoint();
		}
	}	
}


void stkSlicerDisplayHelper::Set3DViewNodeAppearence()
{
	vtkMRMLScene* scene = stkMRMLHelper::mrmlScene();
	if(!scene)	
		return;

	std::vector<vtkMRMLNode*> nodes;
	scene->GetNodesByClass("vtkMRMLViewNode", nodes);

	std::vector<vtkMRMLNode*>::iterator iter;

	for (iter = nodes.begin(); iter != nodes.end(); iter ++)
	{
		vtkMRMLViewNode* viewNode = vtkMRMLViewNode::SafeDownCast(*iter);
		if (viewNode == NULL)
		{
			continue;
		}

		viewNode->SetBackgroundColor(0.2, 0.2, 0.2);
		viewNode->SetBackgroundColor2(0.1, 0.1, 0.1);
		viewNode->SetAxisLabelsVisible(0);
		viewNode->SetBoxVisible(0);
	}
}