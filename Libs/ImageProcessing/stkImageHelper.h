/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkImageProcessingExport.h"

class stkImageHelperPrivate;
class STK_IMAGEPROCESSING_EXPORT stkImageHelper : public QObject
{
	Q_OBJECT

public:
	stkImageHelper(QObject *parent = 0);
	~stkImageHelper();

protected:
	QScopedPointer<stkImageHelperPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkImageHelper);
	Q_DISABLE_COPY(stkImageHelper);	
};
