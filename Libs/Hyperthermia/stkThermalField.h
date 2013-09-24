/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkHyperthermiaExport.h"

class STK_HYPERTHERMIA_EXPORT stkThermalField : public QObject
{
	Q_OBJECT

public:
	stkThermalField(QObject *parent);
	~stkThermalField();

private:
	
};

