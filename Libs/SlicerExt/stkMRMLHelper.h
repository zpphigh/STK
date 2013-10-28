/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkSlicerExtExport.h"

class vtkMRMLScene;

class STK_SLICEREXT_EXPORT stkMRMLHelper
{

public:
	stkMRMLHelper();
	~stkMRMLHelper();	

	static vtkMRMLScene* mrmlScene();

};
