/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkHyperthermiaExport.h"

class stkSurgeryRoutePrivate;
class STK_HYPERTHERMIA_EXPORT stkSurgeryRoute : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double power READ power WRITE setPower)
	Q_PROPERTY(int startTime READ startTime WRITE setStartTime)
	Q_PROPERTY(int endTime READ endTime WRITE setEndTime)
	Q_PROPERTY(double* routeHead READ routeHead WRITE setRouteHead)
	Q_PROPERTY(double* routeRotation READ routeRotation WRITE setRouteRotation)
	Q_PROPERTY(double* routeTail READ routeTail WRITE setRouteTail)
	Q_PROPERTY(int routeID READ routeID WRITE setRouteID)

public:
	stkSurgeryRoute(QObject *parent = 0);
	~stkSurgeryRoute();

	double power() const;
	void setPower(double v);

	int startTime() const;
	void setStartTime(int v);

	int endTime() const;
	void setEndTime(int v);

	virtual double* routeHead() const;
	virtual void setRouteHead(double* v);
	virtual void setRouteHead(double x, double y, double z);

	virtual double* routeRotation() const;
	virtual void setRouteRotation(double* v);
	virtual void setRouteRotation(double x, double y, double z);

	virtual double* routeTail() const;
	virtual void setRouteTail(double* v);
	virtual void setRouteTail(double x, double y, double z);

	virtual int routeID() const;
	virtual void setRouteID(int v);

protected:
	QScopedPointer<stkSurgeryRoutePrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkSurgeryRoute);
	Q_DISABLE_COPY(stkSurgeryRoute);		
};