#include "stkSlicerDisplayHelper.h"

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerAbstractCoreModule.h>
#include <qSlicerAbstractModule.h>

#include <vtkRenderer.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>
#include <vtkMRMLVolumePropertyNode.h>


class stkSlicerDisplayHelperPrivate
{
public:

};

stkSlicerDisplayHelper::stkSlicerDisplayHelper(QObject *parent)
: QObject(parent),d_ptr(new stkSlicerDisplayHelperPrivate)
{
	Q_D(stkSlicerDisplayHelper);

}

stkSlicerDisplayHelper::~stkSlicerDisplayHelper()
{
	Q_D(stkSlicerDisplayHelper);
}



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
			//qThreeDWidget->threeDController()->setVisible(false);
			qMRMLThreeDView* qTreeDView = qThreeDWidget->threeDView();
			if (qTreeDView)
			{
				qTreeDView->resetFocalPoint();
				qTreeDView->renderer()->ResetCamera();
			}
		}
	}	
}
