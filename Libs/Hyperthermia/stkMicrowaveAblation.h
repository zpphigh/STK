/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkHyperthermiaExport.h"

#define m0 1093
#define cc0 3372
#define kk0 0.449
#define mb 1060
#define cb 3770
#define wb0 0.0182
#define Tb 37
#define SatetyBound 30
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ZeroMemory RtlZeroMemory
class stkMicrowaveRoute;
class stkThermalField;
class stkMicrowaveAblationPrivate;
struct HeatFieldRange
{
	double Origin[3];
	double Spacing[3];
	int  Size[3];

	QList<stkMicrowaveRoute*> Routes;
	stkThermalField* pInjuryField3D;
	stkThermalField* pTemperatureField3D;
};

class STK_HYPERTHERMIA_EXPORT stkMicrowaveAblation : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double* ablationOrigin READ ablationOrigin WRITE setAblationOrigin)
	Q_PROPERTY(double* ablationSpacing READ ablationSpacing WRITE setAblationSpacing)
	Q_PROPERTY(int* ablationSize READ ablationSize WRITE setAblationSize)
	Q_PROPERTY(int totalTime READ totalTime WRITE setTotalTime)
	Q_PROPERTY(int fieldTime READ fieldTime WRITE setFieldTime)
	Q_PROPERTY(int* maxPoint READ maxPoint WRITE setMaxPoint)
	Q_PROPERTY(int timeInterval READ timeInterval WRITE setTimeInterval)

public:
	stkMicrowaveAblation(QObject *parent = 0);
	~stkMicrowaveAblation();

	void clear();

	double* ablationOrigin() const;
	void setAblationOrigin(double* v);
	void setAblationOrigin(double x, double y, double z);

	double* ablationSpacing() const;
	void setAblationSpacing(double* v);
	void setAblationSpacing(double x, double y, double z);

	int* ablationSize() const;
	void setAblationSize(int* v);
	void setAblationSize(int x, int y, int z);

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
	float* getThermalField() const;

	void addNeedle(stkMicrowaveRoute* pNeedle);
	int getNumberOfNeedle() const;
	int getNumberOfNeedle(int time ,QList<stkMicrowaveRoute*> &routes) const;
	stkMicrowaveRoute* getNeedle(int i);
	stkMicrowaveRoute* getNeedle(int time, int index, QList<stkMicrowaveRoute*> &routes);

	bool computeMultiRange();
	void addMultiHeatRange();
	QList<HeatFieldRange> getRanges() const;

	bool allocateMemory(int index);
	bool updateMultiFields(int index);

protected:
	QScopedPointer<stkMicrowaveAblationPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkMicrowaveAblation);
	Q_DISABLE_COPY(stkMicrowaveAblation);		
};

