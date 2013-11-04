/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

#include "stkSlicerLayoutWidget.h"
#include "vtkMRMLScene.h"

#include <QMainWindow>

class stkSlicerAppPrivate;
class stkSlicerApp : public QMainWindow
{
	Q_OBJECT

public:
	typedef QMainWindow Superclass;
	stkSlicerApp(QWidget *parent = 0);
	~stkSlicerApp();

protected slots:
	void VolumeDataAdded();

protected:
	QScopedPointer<stkSlicerAppPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkSlicerApp);
	Q_DISABLE_COPY(stkSlicerApp);
};
