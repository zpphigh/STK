#include "stkSlicerDisplayHelper.h"

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>

#include <vtkRenderer.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>

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
