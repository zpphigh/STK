#include "stkFiducialMarkerRegistrationWidget.h"
#include "ui_stkFiducialMarkerRegistrationWidget.h"

#include "qSlicerApplication.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLSelectionNode.h"


class stkFiducialMarkerRegistrationWidgetPrivate : public Ui_stkFiducialMarkerRegistrationWidget
{
public:

	QTimer* PulseGeneratorTimer;
};


stkFiducialMarkerRegistrationWidget::stkFiducialMarkerRegistrationWidget(QWidget *parent)
	: QWidget(parent),d_ptr(new stkFiducialMarkerRegistrationWidgetPrivate)
{
	Q_D(stkFiducialMarkerRegistrationWidget);
	d->setupUi(this);
}


stkFiducialMarkerRegistrationWidget::~stkFiducialMarkerRegistrationWidget()
{

}

void stkFiducialMarkerRegistrationWidget::on_AddFiducialMarkerToolButton_clicked()
{
	Q_D(stkFiducialMarkerRegistrationWidget);

	qSlicerApplication * app = qSlicerApplication::application();
	if (!app) return;

	vtkMRMLScene* scene = app->mrmlScene();
	if (!scene)	return;

	vtkMRMLInteractionNode* interationNode = vtkMRMLInteractionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));
	if(!interationNode)	return;

	vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(scene->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
	if (!selectionNode)	return;

	interationNode->SwitchToPersistentPlaceMode();

	if( d->AddFiducialMarkerToolButton->isChecked()){
		selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");
		interationNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);

	}else{
		interationNode->SwitchToViewTransformMode();
	}
}