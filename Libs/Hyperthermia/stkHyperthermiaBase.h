/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QWidget>


// STK includes
#include "stkHyperthermiaExport.h"

class stkThermalField;
class stkSurgeryRoute;
class stkHyperFieldRange;
class stkHyperthermiaBasePrivate;
class STK_HYPERTHERMIA_EXPORT stkHyperthermiaBase : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(double* hyperOrigin READ hyperOrigin WRITE setHyperOrigin)
	Q_PROPERTY(double* hyperSpacing READ hyperSpacing WRITE setHyperSpacing)
	Q_PROPERTY(int* hyperSize READ hyperSize WRITE setHyperSize)
	Q_PROPERTY(int fieldSize READ fieldSize WRITE setFieldSize)
	Q_PROPERTY(int totalTime READ totalTime WRITE setTotalTime)
	Q_PROPERTY(int timeSteps READ timeSteps WRITE setTimeSteps)
	Q_PROPERTY(int fieldTime READ fieldTime WRITE setFieldTime)
	Q_PROPERTY(int* maxPoint READ maxPoint WRITE setMaxPoint)
	Q_PROPERTY(double timeInterval READ timeInterval WRITE setTimeInterval)
	Q_PROPERTY(int computeProgress READ computeProgress WRITE setComputeProgress)	

public:
	stkHyperthermiaBase(QWidget *parent = 0);
	~stkHyperthermiaBase();

	double* hyperOrigin() const;
	void setHyperOrigin(double* v);
	void setHyperOrigin(double x, double y, double z);

	double* hyperSpacing() const;
	void setHyperSpacing(double* v);
	void setHyperSpacing(double x, double y, double z);

	int* hyperSize() const;
	void setHyperSize(int* v);
	void setHyperSize(int x, int y, int z);

	int fieldSize() const;
	void setFieldSize(int v);

	int totalTime() const;
	void setTotalTime(int v);

	double timeInterval() const;
	void setTimeInterval(double v);

	int fieldTime() const;
	void setFieldTime(int v);

	int* maxPoint() const;
	void setMaxPoint(int* v);
	void setMaxPoint(int x, int y, int z);

	int timeSteps() const;
	void setTimeSteps(int v);

	int computeProgress() const;
	void setComputeProgress(int v);

	void setComputeProgressRange(int v1, int v2);

	virtual void addRoute(stkSurgeryRoute* pRoute);
	virtual int getNumberOfRoutes() const;
	virtual int getNumberOfRoutes(int time ,QList<stkSurgeryRoute*> &routes) const;

	virtual stkSurgeryRoute* getRoute(int i);
	virtual stkSurgeryRoute* getRoute(int time, int index, QList<stkSurgeryRoute*> &routes);

	virtual bool computeMultiRange();
	virtual QList<stkHyperFieldRange*> getRanges() const;
	virtual stkHyperFieldRange* getRange(int i);
	virtual void addRange(stkHyperFieldRange*);
	virtual void clearRanges();
	virtual void clearAllParameters();

	virtual void writeOutputFiles(int* fieldSize, double* pOrigin, float* outputField, int time, QString fieldType = QString("T"));
	virtual void writeOutputTransientFiles(int fieldSize, float* outputField, int time, QString fieldType = QString("T"));

protected:
	QScopedPointer<stkHyperthermiaBasePrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkHyperthermiaBase);
	Q_DISABLE_COPY(stkHyperthermiaBase);	
};
