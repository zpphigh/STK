/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkSlicerExtExport.h"

class stkSlicerExtPrivate;
class STK_SLICEREXT_EXPORT stkSlicerExt : public QObject
{
	Q_OBJECT

public:
	stkSlicerExt(QObject *parent = 0);
	~stkSlicerExt();	


	static void ResetFocalPoint();

protected:
	QScopedPointer<stkSlicerExtPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkSlicerExt);
	Q_DISABLE_COPY(stkSlicerExt);	
};
