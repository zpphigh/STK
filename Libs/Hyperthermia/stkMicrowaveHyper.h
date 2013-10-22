/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once


// STK includes
#include "stkHyperthermiaExport.h"
#include "stkHyperthermiaBase.h"

class stkMicrowaveHyperPrivate;
class STK_HYPERTHERMIA_EXPORT stkMicrowaveHyper : public stkHyperthermiaBase
{
	Q_OBJECT

public:
	stkMicrowaveHyper(QWidget *parent = 0);
	~stkMicrowaveHyper();

	bool allocateMemory(int index);
	bool updateMultiFields(int index);

	float* getThermalField() const;

protected:
	QScopedPointer<stkMicrowaveHyperPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkMicrowaveHyper);
	Q_DISABLE_COPY(stkMicrowaveHyper);	
};
