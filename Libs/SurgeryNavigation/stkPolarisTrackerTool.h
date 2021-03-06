/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "igstkPolarisTracker.h"
#include "igstkTrackerTool.h"
#include "stkTrackerTool.h"

class stkPolarisTracker;
class stkPolarisTrackerToolPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkPolarisTrackerTool : public stkTrackerTool
{
	Q_OBJECT

public:
	stkPolarisTrackerTool(QString _name,  QString  romFile, igstk::PolarisTracker::Pointer tracker);
	~stkPolarisTrackerTool();

	virtual void SetSensorOffset(float x, float y, float z);
	virtual void DetachFromTracker();
	virtual bool FetchToolPosition();
	virtual bool GetPosition(float* pos, float* quat);

protected:
	QScopedPointer<stkPolarisTrackerToolPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkPolarisTrackerTool);
	Q_DISABLE_COPY(stkPolarisTrackerTool);	
};

