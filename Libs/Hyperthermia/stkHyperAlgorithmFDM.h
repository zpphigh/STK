/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QWidget>

// STK includes
#include "stkHyperthermiaExport.h"

#define m0 1093
#define cc0 3372
#define kk0 0.449
#define mb 1060
#define cb 3770
#define wb0 0.0182
#define Tb 37
#define SAFE_DELETE_ARRAY(p) { if(p) {delete[] (p); (p) = NULL;}}
class stkHyperAlgorithmFDMPrivate;
class STK_HYPERTHERMIA_EXPORT stkHyperAlgorithmFDM : public QObject
{
	Q_OBJECT

public:
	stkHyperAlgorithmFDM(QObject *parent = 0);
	~stkHyperAlgorithmFDM();

	static bool updateHyperFields(int* size, double* spacing, 
		int timeInterval, float* pEnergyField, 
		float* pInjuryField, float* InjuryFieldOld, 
		float* TempratureField, float* TempratureFieldOld);

protected:
	QScopedPointer<stkHyperAlgorithmFDMPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkHyperAlgorithmFDM);
	Q_DISABLE_COPY(stkHyperAlgorithmFDM);
};
