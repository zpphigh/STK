/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QThread>

class stkIGTLServerThreadPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkIGTLServerThread : public QThread
{
	Q_OBJECT

public:
	stkIGTLServerThread(QObject *parent = 0);
	~stkIGTLServerThread();

	void AbortThread();

protected:
	void run();

protected:
	QScopedPointer<stkIGTLServerThreadPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkIGTLServerThread);
	Q_DISABLE_COPY(stkIGTLServerThread);	
};
