/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// Qt includes
#include <QObject>

class stkIGTLImageClientPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkIGTLImageClient : public QObject
{
	Q_OBJECT
public:

	stkIGTLImageClient(QObject *parent = 0);
	 ~stkIGTLImageClient();

  bool isServerConnected();
  bool ConnectIGTLServer(const char* hostname, int port);
  void DisconnectIGTLServer();


  void SetImageSize(int*);
  void SetImageSpacing(float*);
  void SetImageOrigin(float*);
  int* GetImageSize();
  float* GetImageSpacing();
  float* GetImageOrigin();

  void SetImageSize(int x, int y, int z);
  void SetImageSpacing(float x, float y, float z);
  void SetImageOrigin(float x, float y, float z);
  bool SendImage( unsigned char* bufPtr);

  void allocateImageMessage();

protected:
	QScopedPointer<stkIGTLImageClientPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkIGTLImageClient);
	Q_DISABLE_COPY(stkIGTLImageClient);
};

