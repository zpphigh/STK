/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "igstkAuroraTracker.h"
#include "igstkTrackerTool.h"
#include "stkTrackerTool.h"

class stkAuroraTracker;
class stkAuroraTrackerToolPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkAuroraTrackerTool : public stkTrackerTool
{
	Q_OBJECT

public:
	stkAuroraTrackerTool(QString _name, int toolDof ,int portNumber, int channelNumber, igstk::AuroraTracker::Pointer tracker);
	~stkAuroraTrackerTool();

	virtual void SetSensorOffset(float x, float y, float z);
	virtual void DetachFromTracker();
	virtual bool FetchToolPosition();
	virtual bool GetPosition(float* pos, float* quat);

protected:
	QScopedPointer<stkAuroraTrackerToolPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkAuroraTrackerTool);
	Q_DISABLE_COPY(stkAuroraTrackerTool);	
};

