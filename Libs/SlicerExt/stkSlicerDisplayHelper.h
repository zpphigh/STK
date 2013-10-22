/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkSlicerExtExport.h"

class stkSlicerDisplayHelperPrivate;
class STK_SLICEREXT_EXPORT stkSlicerDisplayHelper : public QObject
{
	Q_OBJECT

public:
	stkSlicerDisplayHelper(QObject *parent = 0);
	~stkSlicerDisplayHelper();	


	static void ResetFocalPoint();

protected:
	QScopedPointer<stkSlicerDisplayHelperPrivate> d_ptr;

private:	
	Q_DECLARE_PRIVATE(stkSlicerDisplayHelper);
	Q_DISABLE_COPY(stkSlicerDisplayHelper);	
};
