/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"
#include "ui_stkUltrasoundImageFusionWidget.h"

class STK_SURGERYNAVIGATION_EXPORT stkUltrasoundImageFusionWidget : public QWidget
{
	Q_OBJECT

public:
	stkUltrasoundImageFusionWidget(QWidget *parent = 0);
	~stkUltrasoundImageFusionWidget();

private:
	Ui::stkUltrasoundImageFusionWidget ui;
};

