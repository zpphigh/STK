/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "vtkMRMLNode.h"

class STK_SURGERYNAVIGATION_EXPORT stkMRMLIGTLImageClientNode : public vtkMRMLNode
{
 public:
  static stkMRMLIGTLImageClientNode *New();
  vtkTypeMacro(stkMRMLIGTLImageClientNode,vtkMRMLNode);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "IGTLImageClient";}

  bool ConnectIGTLServer(const char* hostname, int port);
  void DisconnectIGTLServer();

  void SetImageSize(int x, int y, int z);
  void SetImageSpacing(float x, float y, float z);
  void SetImageOrigin(float x, float y, float z);
  bool SendImage( unsigned char* bufPtr, int bufSize);

 protected:
  stkMRMLIGTLImageClientNode();
  ~stkMRMLIGTLImageClientNode();
  stkMRMLIGTLImageClientNode(const stkMRMLIGTLImageClientNode&);
  void operator=(const stkMRMLIGTLImageClientNode&);

private:
  igtl::ClientSocket::Pointer imageClientSocket;
  igtl::ImageMessage::Pointer imageMessage;
  int imageSize[3];
  float imageSpacing[3];
  float imageOrigin[3];
  int imageBufSize;

  void allocateImageMessage();
};

