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

public slots:
	void on_AddFiducialMarkerToolButton_clicked();

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
