/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkHyperthermiaExport.h"

class stkSurgeryRoute;
class stkThermalField;
class stkHyperFieldRangePrivate;
class STK_HYPERTHERMIA_EXPORT stkHyperFieldRange : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double* rangeOrigin READ rangeOrigin WRITE setRangeOrigin)
	Q_PROPERTY(double* rangeSpacing READ rangeSpacing WRITE setRangeSpacing)
	Q_PROPERTY(int* rangeSize READ rangeSize WRITE setRangeSize)

public:
	stkHyperFieldRange(QObject *parent = 0);
	~stkHyperFieldRange();

	double* rangeOrigin() const;
	void setRangeOrigin(double* v);
	void setRangeOrigin(double x, double y, double z);

	double* rangeSpacing() const;
	void setRangeSpacing(double* v);
	void setRangeSpacing(double x, double y, double z);

	int* rangeSize() const;
	void setRangeSize(int* v);
	void setRangeSize(int x, int y, int z);

	QList<stkSurgeryRoute*> getRoutes() const;
	void clearRoutes();
	void addRoute(stkSurgeryRoute* route);
	int getNumberOfRoutes() const;
	stkSurgeryRoute* getRoute(int i);

	stkThermalField* getThermalField3D();
	stkThermalField* getInjuryField3D();

protected:
	QScopedPointer<stkHyperFieldRangePrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkHyperFieldRange);
	Q_DISABLE_COPY(stkHyperFieldRange);	
	
};
