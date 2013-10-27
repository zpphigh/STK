/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  this class if modified from the OpenIGTLinkIF class vtkIGTLCircularBuffer

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"


// VTK includes
#include <vtkObject.h>

// OpenIGTLink includes
#include <igtlMessageBase.h>


// STD includes
#include <string>

#define IGTLCB_CIRC_BUFFER_SIZE    3

class vtkMutexLock;

class STK_SURGERYNAVIGATION_EXPORT stkIGTLCircularBuffer : public vtkObject
{
 public:

  static stkIGTLCircularBuffer *New();
  vtkTypeRevisionMacro(stkIGTLCircularBuffer,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  int GetNumberOfBuffer() { return IGTLCB_CIRC_BUFFER_SIZE; }

  int            StartPush();
  void           EndPush();
  igtl::MessageBase::Pointer GetPushBuffer();

  int            StartPull();
  void           EndPull();
  igtl::MessageBase::Pointer GetPullBuffer();

  int            IsUpdated() { return this->UpdateFlag; };

 protected:
  stkIGTLCircularBuffer();
  virtual ~stkIGTLCircularBuffer();

 protected:

  vtkMutexLock*      Mutex;
  int                Last;        // updated by connector thread
  int                InPush;      // updated by connector thread
  int                InUse;       // updated by main thread

  int                UpdateFlag;  // non-zero if updated since StartPull() has called

  //每个有下面四项
  std::string        DeviceType[IGTLCB_CIRC_BUFFER_SIZE];
  long long          Size[IGTLCB_CIRC_BUFFER_SIZE];
  unsigned char*     Data[IGTLCB_CIRC_BUFFER_SIZE];
  igtl::MessageBase::Pointer Messages[IGTLCB_CIRC_BUFFER_SIZE];

};
