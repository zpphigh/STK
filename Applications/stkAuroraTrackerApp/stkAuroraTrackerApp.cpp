#include "stkAuroraTrackerApp.h"
#include "ui_stkAuroraTrackerApp.h"

//Qt includes
#include <QFrame>
#include <QTimer>

//Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "stkSlicerDisplayHelper.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "stkMRMLHelper.h"

#include "stkTrackerThread.h"
#include "stkTrackerTool.h"
#include "stkIGTLServerManager.h"
#include "qMRMLNodeFactory.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLSliceLogic.h"
#include <vtkMatrix4x4.h>

class stkAuroraTrackerAppPrivate : public Ui_stkAuroraTrackerApp
{
	Q_DECLARE_PUBLIC(stkAuroraTrackerApp);
protected:
	stkAuroraTrackerApp* const q_ptr;

public:
	typedef stkAuroraTrackerAppPrivate Self;
	stkAuroraTrackerAppPrivate(stkAuroraTrackerApp& object);
	void setupUi(QMainWindow * mainWindow);
	
	qSlicerLayoutManager*	LayoutManager;
	stkTrackerThread TrackerThread;
	stkIGTLServerManager* IGTLServerManager;
	QTimer importDataAndEventsTimer;
};

stkAuroraTrackerAppPrivate::stkAuroraTrackerAppPrivate(stkAuroraTrackerApp& object)
: q_ptr(&object)
{
	this->LayoutManager = 0;
}



stkAuroraTrackerApp::stkAuroraTrackerApp(QWidget *parent)
	: Superclass(parent),d_ptr(new stkAuroraTrackerAppPrivate(*this))
{
	Q_D(stkAuroraTrackerApp);
	d->setupUi(this);

	this->setAttribute(Qt::WA_DeleteOnClose);

	connect(d->SlicerDataWidget,SIGNAL(VolumeDataAdded()),this,SLOT(VolumeDataAdded()));

	//IGT

	d->UltrasoundImageFusionWidget->setEnabled(false);

	d->IGTLServerManager = new stkIGTLServerManager(this);
	d->IGTLServerManager->StartIGTLServer();
	d->IGTLServerManager->StartImageServer();
	this->connect(&d->importDataAndEventsTimer, SIGNAL(timeout()),  d->IGTLServerManager, SLOT(importDataAndEvents()));
	d->importDataAndEventsTimer.start(5);

	connect(&(d->TrackerThread),SIGNAL(TrackingStarted()),this,SLOT(TrackingStarted()));
	connect(&(d->TrackerThread),SIGNAL(TrackingStoped()),this,SLOT(TrackingStoped()));


	//启动Tracker线程
	d->TrackerThread.SetIGTServer("localhost",18944);
	d->TrackerThread.InitAuroraTracker(3);
	d->TrackerThread.start();  
}

stkAuroraTrackerApp::~stkAuroraTrackerApp()
{
	Q_D(stkAuroraTrackerApp);
	d->importDataAndEventsTimer.stop();

	//安全关闭Tracker线程
	d->TrackerThread.Abort();
	d->TrackerThread.disconnect();
	d->TrackerThread.quit();
	d->TrackerThread.wait();
	d->TrackerThread.deleteLater();
}


void stkAuroraTrackerAppPrivate::setupUi(QMainWindow * mainWindow)
{
	Q_Q(stkAuroraTrackerApp);

	this->Ui_stkAuroraTrackerApp::setupUi(mainWindow);

	// Color of the spacing between views:
	QFrame* layoutFrame = new QFrame(this->slicerWidget);
	layoutFrame->setObjectName("CentralWidgetLayoutFrame");
	QHBoxLayout* centralLayout = new QHBoxLayout(this->slicerWidget);
	centralLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addWidget(layoutFrame);

	QColor windowColor = this->slicerWidget->palette().color(QPalette::Window);
	QPalette centralPalette = this->slicerWidget->palette();
	centralPalette.setColor(QPalette::Window, QColor(95, 95, 113));
	this->slicerWidget->setAutoFillBackground(true);
	this->slicerWidget->setPalette(centralPalette);

	// Restore the palette for the children
	centralPalette.setColor(QPalette::Window, windowColor);
	layoutFrame->setPalette(centralPalette);
	layoutFrame->setAttribute(Qt::WA_NoSystemBackground, true);
	layoutFrame->setAttribute(Qt::WA_TranslucentBackground, true);


	//----------------------------------------------------------------------------
	// Layout Manager
	//----------------------------------------------------------------------------
	// Instanciate and assign the layout manager to the slicer application
	this->LayoutManager = new qSlicerLayoutManager(layoutFrame);
	qSlicerApplication::application()->setLayoutManager(this->LayoutManager);

	// Layout manager should also listen the MRML scene
	this->LayoutManager->setMRMLScene(qSlicerApplication::application()->mrmlScene());
	QObject::connect(qSlicerApplication::application(), SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), this->LayoutManager, SLOT(setMRMLScene(vtkMRMLScene*)));

	//3d View Appearence setting
	stkSlicerDisplayHelper::Set3DViewNodeAppearence();
}


void stkAuroraTrackerApp::VolumeDataAdded()
{
	Q_D(stkAuroraTrackerApp);

	vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetNthNodeByClass(0,"vtkMRMLScalarVolumeNode"));
	if(!node)
		return;

	d->SliceNodeWidget->setMRMLVolumeNode(node);
	d->VolumeDisplayWidget->setMRMLVolumeNode(node);
	d->VolumeRenderingWidget->setMRMLVolumeNode(node);
}



void stkAuroraTrackerApp::TrackingStarted()
{
	Q_D(stkAuroraTrackerApp);

	d->UltrasoundImageFusionWidget->setEnabled(true);

	stkTrackerTool* CalibrationTool = d->TrackerThread.GetTrackerTool("CalibrationTool");
	if(CalibrationTool)
	{
		this->connect(CalibrationTool, SIGNAL(dataValidChanged(bool)),d->FiducialMarkerRegistrationWidget,SLOT(setCalibrationToolDataValid(bool)));
	}

	stkTrackerTool* UltrasoundTool = d->TrackerThread.GetTrackerTool("UltrasoundTool");
	if(UltrasoundTool)
	{
		connect(UltrasoundTool, SIGNAL(dataValidChanged(bool)),d->UltrasoundImageFusionWidget,SLOT(setUltrasoundToolDataValid(bool)));
	}

}

