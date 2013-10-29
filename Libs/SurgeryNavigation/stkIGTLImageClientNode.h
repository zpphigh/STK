/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "vtkMRMLNode.h"

class STK_SURGERYNAVIGATION_EXPORT stkIGTLImageClientNode : public vtkMRMLNode
{
 public:
  static stkIGTLImageClientNode *New();
  vtkTypeMacro(stkIGTLImageClientNode,vtkMRMLNode);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "IGTLImageClient";};


 protected:
  stkIGTLImageClientNode();
  ~stkIGTLImageClientNode();
  stkIGTLImageClientNode(const stkIGTLImageClientNode&);
  void operator=(const stkIGTLImageClientNode&);
};

