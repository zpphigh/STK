/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QObject>

// IGSTK includes
#include "igtlPositionMessage.h"
//#include "igstkTrackerTool.h"

class stkTrackerToolPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkTrackerTool : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(bool dataValid READ dataValid WRITE setDataValid NOTIFY dataValidChanged)

public:
	stkTrackerTool(QObject *parent);
	~stkTrackerTool();

	QString name() const;
	void setName(QString);

	bool dataValid() const;
	void setDataValid(bool);

	virtual void SetSensorOffset(float x, float y, float z) = 0;
	virtual void DetachFromTracker() = 0;
	virtual bool FetchToolPosition() = 0;
	virtual bool GetPosition(float* pos, float* quat) = 0;
  
	//virtual igstk::TrackerTool::Pointer GetTrackerToolPointer() = 0;

signals:
	void dataValidChanged(bool);

protected:
	QScopedPointer<stkTrackerToolPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkTrackerTool);
	Q_DISABLE_COPY(stkTrackerTool);
};

