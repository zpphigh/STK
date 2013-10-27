/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"


// OpenIGTLinkIF MRML includes
#include "stkIGTLToMRMLBase.h"


// OpenIGTLink includes
#include <igtlPositionMessage.h>

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>
class vtkMRMLModelNode;

class STK_SURGERYNAVIGATION_EXPORT stkIGTLToMRMLPosition : public stkIGTLToMRMLBase
{
 public:

  static stkIGTLToMRMLPosition *New();
  vtkTypeRevisionMacro(stkIGTLToMRMLPosition,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POSITION"; };
  virtual const char*  GetMRMLName() { return "LinearTransform"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);

  virtual int IsVisible() { return 1; };
  virtual void SetVisibility(int sw, vtkMRMLScene * scene, vtkMRMLNode * node);


 protected:
  stkIGTLToMRMLPosition();
  ~stkIGTLToMRMLPosition();

   vtkMRMLModelNode* AddLocatorModel(vtkMRMLScene * scene, const char* nodeName, double r, double g, double b);

 protected:
  igtl::PositionMessage::Pointer OutPositionMsg;

};
