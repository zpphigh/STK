/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

class vtkMRMLScene;
class vtkMRMLNode;
class STK_SLICEREXT_EXPORT stkMRMLHelper
{
public:
	static vtkMRMLScene* mrmlScene();

	// find a mrml node by a given name, if more than one node has the same name, NullPtr is returned
	static vtkMRMLNode* GetSingleMRMLNodeByName(char* name);
};
