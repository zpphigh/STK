/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkHyperthermiaBase.h"
#include "ui_stkHyperthermiaBase.h"

#define SatetyBound 30
class stkSurgeryRoute;
class stkHyperFieldRange;
class stkHyperthermiaBasePrivate : public Ui_stkHyperthermiaBase
{
public:
	stkHyperthermiaBasePrivate();
	~stkHyperthermiaBasePrivate();

	void clear();

	bool computeSingleRange(int i, stkHyperFieldRange* SingleRange);

	QList<stkSurgeryRoute*> HyperRoutes;
	int TotalTime;
	int FieldTime;
	double Origin[3];
	double Spacing[3];
	int Size[3];
	int MaxPoint[3];
	double TimeInterval;
	int TimeSteps;
	int FieldSize;

	int ComputeProgress;

	QList<stkHyperFieldRange*> Ranges;	
};
