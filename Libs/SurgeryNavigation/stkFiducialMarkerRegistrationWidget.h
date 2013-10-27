/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QWidget>


class stkFiducialMarkerRegistrationWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkFiducialMarkerRegistrationWidget : public QWidget
{
	Q_OBJECT

public:
	stkFiducialMarkerRegistrationWidget(QWidget *parent = 0);
	~stkFiducialMarkerRegistrationWidget();

protected:
	QScopedPointer<stkFiducialMarkerRegistrationWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkFiducialMarkerRegistrationWidget);
	Q_DISABLE_COPY(stkFiducialMarkerRegistrationWidget);
};
