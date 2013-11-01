/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QObject>

class stkIGTLServerManagerPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkIGTLServerManager : public QObject
{
	Q_OBJECT

public:
	stkIGTLServerManager(QObject *parent = 0);
	~stkIGTLServerManager();

	void StartIGTLServer();
	void StopIGTServer();

public slots:
	void importDataAndEvents();

protected:
	QScopedPointer<stkIGTLServerManagerPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkIGTLServerManager);
	Q_DISABLE_COPY(stkIGTLServerManager);	
};
