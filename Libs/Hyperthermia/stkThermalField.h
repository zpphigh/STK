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
#include "math.h"

class stkThermalFieldPrivate;
class STK_HYPERTHERMIA_EXPORT stkThermalField : public QObject
{
	Q_OBJECT
		Q_PROPERTY(double* fieldOrigin READ fieldOrigin WRITE setFieldOrigin)
		Q_PROPERTY(double* fieldSpacing READ fieldSpacing WRITE setFieldSpacing)
		Q_PROPERTY(int* fieldSize READ fieldSize WRITE setFieldSize)
		Q_PROPERTY(int totalTime READ totalTime WRITE setTotalTime)
		Q_PROPERTY(int fieldTime READ fieldTime WRITE setFieldTime)
		Q_PROPERTY(int* maxPoint READ maxPoint WRITE setMaxPoint)
		Q_PROPERTY(int timeInterval READ timeInterval WRITE setTimeInterval)

public:
	stkThermalField(QObject *parent = 0);
	~stkThermalField();	

	double* fieldOrigin() const;
	void setFieldOrigin(double* v);
	void setFieldOrigin(double x, double y, double z);

	double* fieldSpacing() const;
	void setFieldSpacing(double* v);
	void setFieldSpacing(double x, double y, double z);

	int* fieldSize() const;
	void setFieldSize(int* v);
	void setFieldSize(int x, int y, int z);

	int totalTime() const;
	void setTotalTime(int v);

	int timeInterval() const;
	void setTimeInterval(int v);

	int fieldTime() const;
	void setFieldTime(int v);

	int* maxPoint() const;
	void setMaxPoint(int* v);
	void setMaxPoint(int x, int y, int z);

	int timeSteps() const;
	float* fieldBuffer() const;
	virtual void allocateMemory();
	virtual void clear();
	virtual void update();

protected:
	QScopedPointer<stkThermalFieldPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkThermalField);
	Q_DISABLE_COPY(stkThermalField);	
};
