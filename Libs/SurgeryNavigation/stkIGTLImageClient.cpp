
#include "stkIGTLImageClient.h"
#include <igtlServerSocket.h>
#include <igtlOSUtil.h>
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>


class stkIGTLImageClientPrivate 
{
public:
	igtl::ClientSocket::Pointer ImageClientSocket;
	igtl::ImageMessage::Pointer ImageMessage;
	int ImageSize[3];
	float ImageSpacing[3];
	float ImageOrigin[3];
	int ImageBufSize;
};


//----------------------------------------------------------------------------
stkIGTLImageClient::stkIGTLImageClient(QObject *parent)
:QObject(parent),d_ptr(new stkIGTLImageClientPrivate)
{
	Q_D(stkIGTLImageClient);

	d->ImageClientSocket = igtl::ClientSocket::New();
	d->ImageMessage = igtl::ImageMessage::New();

	d->ImageSize[0] = 640;
	d->ImageSize[1] = 480;
	d->ImageSize[2] = 1;

	d->ImageSpacing[0] = 1;
	d->ImageSpacing[1] = 1;
	d->ImageSpacing[2] = 5;

	d->ImageOrigin[0] = 0;
	d->ImageOrigin[1] = 0;
	d->ImageOrigin[2] = 0;

	d->ImageBufSize = 0;
}

//----------------------------------------------------------------------------
stkIGTLImageClient::~stkIGTLImageClient()
{
 
}

bool stkIGTLImageClient::isServerConnected()
{
	Q_D(stkIGTLImageClient);

	if( d->ImageClientSocket.IsNull())
		return false;

	return d->ImageClientSocket->GetConnected();
}

bool stkIGTLImageClient::ConnectIGTLServer(const char* hostname, int port)
{
	Q_D(stkIGTLImageClient);

	if(d->ImageClientSocket.IsNull())
		return false;

	// return if the client is connected
	if(d->ImageClientSocket->GetConnected())
		return true;

	if(d->ImageClientSocket->ConnectToServer(hostname,port))
		return false;

	return true;
}

void stkIGTLImageClient::DisconnectIGTLServer()
{
	Q_D(stkIGTLImageClient);

	if(!d->ImageClientSocket)
		return;

	if(d->ImageClientSocket->GetConnected())
		d->ImageClientSocket->CloseSocket();
}

void stkIGTLImageClient::SetImageSize(int* s)
{
	SetImageSize(s[0],s[1],s[2]);
}


void stkIGTLImageClient::SetImageSpacing(float* s)
{
	SetImageSpacing(s[0],s[1],s[2]);
}


void stkIGTLImageClient::SetImageOrigin(float* o)
{
	SetImageOrigin(o[0],o[1],o[2]);
}

int* stkIGTLImageClient::GetImageSize()
{
	Q_D(stkIGTLImageClient);
	return d->ImageSize;
}

float* stkIGTLImageClient::GetImageSpacing()
{
	Q_D(stkIGTLImageClient);
	return d->ImageSpacing;
}


float* stkIGTLImageClient::GetImageOrigin()
{
	Q_D(stkIGTLImageClient);
	return d->ImageOrigin;
}



void stkIGTLImageClient::SetImageSize(int x, int y, int z)
{
	Q_D(stkIGTLImageClient);

	int size[3];
	d->ImageMessage->GetDimensions(size);

	if( size[0] == x && size[1] == y && size[2] == z)
		return;

	d->ImageSize[0] = x;
	d->ImageSize[1] = y;
	d->ImageSize[2] = z;

	d->ImageBufSize = d->ImageMessage->GetScalarSize()*x*y*z;

	allocateImageMessage();
}

void stkIGTLImageClient::SetImageSpacing(float x, float y, float z)
{
	Q_D(stkIGTLImageClient);

	d->ImageSpacing[0] = x;
	d->ImageSpacing[1] = y;
	d->ImageSpacing[2] = z;

	d->ImageMessage->SetSpacing(d->ImageSpacing);
}

void stkIGTLImageClient::SetImageOrigin(float x, float y, float z)
{
	Q_D(stkIGTLImageClient);

	d->ImageOrigin[0] = x;
	d->ImageOrigin[1] = y;
	d->ImageOrigin[2] = z;

	d->ImageMessage->SetOrigin(d->ImageOrigin);
}

void stkIGTLImageClient::allocateImageMessage()
{
	Q_D(stkIGTLImageClient);

	d->ImageMessage = igtl::ImageMessage::New();

	int   svoffset[] = {0, 0, 0}; // sub-volume offset

	igtl::Matrix4x4 matrix;
	matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;

	
	d->ImageMessage->SetMatrix(matrix);
	d->ImageMessage->SetDimensions(d->ImageSize);
	d->ImageMessage->SetSpacing(d->ImageSpacing);
	d->ImageMessage->SetOrigin(d->ImageOrigin);
	d->ImageMessage->SetNumComponents(1);
	d->ImageMessage->SetScalarTypeToUint8();
	d->ImageMessage->SetDeviceName("RTImage");
	d->ImageMessage->SetSubVolume(d->ImageSize, svoffset);

	d->ImageBufSize =d->ImageMessage->GetScalarSize()*d->ImageSize[0]*d->ImageSize[1]*d->ImageSize[2];
	d->ImageMessage->AllocateScalars();
}


bool stkIGTLImageClient::SendImage( unsigned char* bufPtr)
{
	Q_D(stkIGTLImageClient);

	memcpy(d->ImageMessage->GetScalarPointer(),bufPtr,d->ImageBufSize);
	d->ImageMessage->Pack();

	d->ImageClientSocket->Send(d->ImageMessage->GetPackPointer(), d->ImageMessage->GetPackSize());

	return true;
}