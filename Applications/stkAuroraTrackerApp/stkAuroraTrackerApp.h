#pragma once
#include "stkSlicerLayoutWidget.h"
#include "vtkMRMLScene.h"
#include <QMainWindow>

class stkAuroraTrackerAppPrivate;
class stkAuroraTrackerApp : public QMainWindow
{
	Q_OBJECT

public:
	typedef QMainWindow Superclass;
	stkAuroraTrackerApp(QWidget *parent = 0);
	~stkAuroraTrackerApp();

public slots:
	void VolumeDataAdded();
	void TrackingStarted();

protected:
	QScopedPointer<stkAuroraTrackerAppPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkAuroraTrackerApp);
	Q_DISABLE_COPY(stkAuroraTrackerApp);
};
