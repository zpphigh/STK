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
#include <igtlImageMessage.h>

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class vtkMRMLVolumeNode;

class STK_SURGERYNAVIGATION_EXPORT stkIGTLToMRMLImage : public stkIGTLToMRMLBase
{
 public:

  static stkIGTLToMRMLImage *New();
  vtkTypeRevisionMacro(stkIGTLToMRMLImage,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "IMAGE"; };
  virtual const char*  GetMRMLName() { return "Volume"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  stkIGTLToMRMLImage();
  ~stkIGTLToMRMLImage();

  void CenterImage(vtkMRMLVolumeNode *volumeNode);

 protected:
  igtl::ImageMessage::Pointer OutImageMessage;
  igtl::GetImageMessage::Pointer GetImageMessage;
};


