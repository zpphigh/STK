/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkHyperthermiaExport.h"
#include "stkSurgeryRoute.h"

class stkMicrowaveRoutePrivate;
class STK_HYPERTHERMIA_EXPORT stkMicrowaveRoute : public stkSurgeryRoute
{
	Q_OBJECT
	Q_PROPERTY(double ct READ ct WRITE setCt)
	Q_PROPERTY(double z0 READ z0 WRITE setZ0)

public:
	stkMicrowaveRoute(QObject *parent = 0);
	~stkMicrowaveRoute();

	double ct() const;
	void setCt(double v);

	double z0() const;
	void setZ0(double v);

protected:
	QScopedPointer<stkMicrowaveRoutePrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkMicrowaveRoute);
	Q_DISABLE_COPY(stkMicrowaveRoute);	
};
