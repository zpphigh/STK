/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QWidget>

// STK includes
#include "stkHyperthermiaExport.h"

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
