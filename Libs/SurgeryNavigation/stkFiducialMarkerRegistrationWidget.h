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
class stkTrackerTool;

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

	stkTrackerTool* GetTrackerTool(QString name);

public slots:
	bool StartTracking();
	void StopTracking();

	void StartIGTLServer();

signals:
	void TrackingStarted();
	void TrackingStoped();

protected slots:
	void on_AddFiducialMarkerToolButton_clicked();
	void on_DeleteFiducialMarkerToolButton_clicked();
	void on_ClearFiducialMarkerToolButton_clicked();
	void on_StartTrackingToolButton_clicked();
	void on_CalibrationToolButton_clicked();

	void setCalibrationToolDataValid(bool valid);
	void onMarkupNodeAdded();
	void onActiveMarkupsNodeMarkupAddedEvent();
	void onActiveMarkupsNodePointModifiedEvent(vtkObject *caller, vtkObject *callData);
	void onActiveMarkupsNodeMarkupRemovedEvent();
	void onActiveMarkupsNodeDisplayModifiedEvent();
	void updateFiducialMarkers();

	void importDataAndEvents();

private:
	void observeMarkupsNode(vtkMRMLNode *markupsNode);


protected:
	QScopedPointer<stkFiducialMarkerRegistrationWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkFiducialMarkerRegistrationWidget);
	Q_DISABLE_COPY(stkFiducialMarkerRegistrationWidget);
};
