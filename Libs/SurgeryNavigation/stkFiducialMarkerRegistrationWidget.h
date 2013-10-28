/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include "qSlicerWidget.h"

class vtkMRMLNode;

class stkFiducialMarkerRegistrationWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkFiducialMarkerRegistrationWidget : public qSlicerWidget
{
	Q_OBJECT
	
public:
	typedef qSlicerWidget Superclass;
	stkFiducialMarkerRegistrationWidget(QWidget *parent = 0);
	~stkFiducialMarkerRegistrationWidget();



	//The following three function is used to configure which Tracker to use and related parameters
	void UseTrackerAurora(int comPort);
	void UseTrackerPolaris(int comPort);
	void UseTrackerAscension();

	void ImportFromCircularBuffers();
	void ImportEvents(); // check if there are any events in the connectors that should be invoked in the main thread (such as connected, disconnected)


public slots:
	bool StartTracking();
	void StopTracking();

	void StartIGTLServer();

protected slots:
	void on_AddFiducialMarkerToolButton_clicked();
	void on_DeleteFiducialMarkerToolButton_clicked();
	void on_ClearFiducialMarkerToolButton_clicked();
	void on_StartTrackingToolButton_clicked();

	void setCalibrationToolDataValid(bool valid);
	void onMarkupNodeAdded();
	void onActiveMarkupsNodeMarkupAddedEvent();
	void onActiveMarkupsNodePointModifiedEvent(vtkObject *caller, vtkObject *callData);
	void onActiveMarkupsNodeMarkupRemovedEvent();
	void onActiveMarkupsNodeDisplayModifiedEvent();
	void updateFiducialMarkers();



private:
	void observeMarkupsNode(vtkMRMLNode *markupsNode);


protected:
	QScopedPointer<stkFiducialMarkerRegistrationWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkFiducialMarkerRegistrationWidget);
	Q_DISABLE_COPY(stkFiducialMarkerRegistrationWidget);
};
