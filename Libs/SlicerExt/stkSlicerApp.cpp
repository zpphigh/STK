#include "stkSlicerApp.h"
#include "ui_stkSlicerApp.h"

//Qt includes
#include <QFrame>

//Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "stkSlicerDisplayHelper.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "stkMRMLHelper.h"


class stkSlicerAppPrivate : public Ui_stkSlicerApp
{
	Q_DECLARE_PUBLIC(stkSlicerApp);
protected:
	stkSlicerApp* const q_ptr;

public:
	typedef stkSlicerAppPrivate Self;
	stkSlicerAppPrivate(stkSlicerApp& object);
	void setupUi(QMainWindow * mainWindow);

	qSlicerLayoutManager*	LayoutManager;
};

stkSlicerAppPrivate::stkSlicerAppPrivate(stkSlicerApp& object)
: q_ptr(&object)
{
	this->LayoutManager = 0;
}



stkSlicerApp::stkSlicerApp(QWidget *parent)
	: Superclass(parent),d_ptr(new stkSlicerAppPrivate(*this))
{
	Q_D(stkSlicerApp);
	d->setupUi(this);

	connect(d->SlicerDataWidget,SIGNAL(VolumeDataAdded()),this,SLOT(VolumeDataAdded()));

	this->setAttribute(Qt::WA_DeleteOnClose);
}

stkSlicerApp::~stkSlicerApp()
{

}


void stkSlicerAppPrivate::setupUi(QMainWindow * mainWindow)
{
	Q_Q(stkSlicerApp);

	this->Ui_stkSlicerApp::setupUi(mainWindow);

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


void stkSlicerApp::VolumeDataAdded()
{
	Q_D(stkSlicerApp);

	vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(qSlicerApplication::application()->mrmlScene()->GetNthNodeByClass(0,"vtkMRMLScalarVolumeNode"));
	if(!node)
		return;

	d->VolumeDisplayWidget->setMRMLVolumeNode(node);
}