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
#include <igtlTransformMessage.h>

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class vtkMRMLModelNode;


class STK_SURGERYNAVIGATION_EXPORT stkIGTLToMRMLLinearTransform : public stkIGTLToMRMLBase
{
 public:

  static stkIGTLToMRMLLinearTransform *New();
  vtkTypeRevisionMacro(stkIGTLToMRMLLinearTransform,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "TRANSFORM"; };
  virtual const char*  GetMRMLName() { return "LinearTransform"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);

  virtual int IsVisible() { return 1; };
  virtual void SetVisibility(int sw, vtkMRMLScene * scene, vtkMRMLNode * node);

 protected:
  stkIGTLToMRMLLinearTransform();
  ~stkIGTLToMRMLLinearTransform();

  vtkMRMLModelNode* AddLocatorModel(vtkMRMLScene * scene, const char* nodeName, double r, double g, double b);

 protected:
  igtl::TransformMessage::Pointer OutTransformMsg;

};

