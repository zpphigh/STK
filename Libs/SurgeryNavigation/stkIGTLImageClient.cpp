
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
	igtl::ClientSocket::Pointer imageClientSocket;
	igtl::ImageMessage::Pointer imageMessage;
	int imageSize[3];
	float imageSpacing[3];
	float imageOrigin[3];
	int imageBufSize;
};


//----------------------------------------------------------------------------
stkIGTLImageClient::stkIGTLImageClient(QObject *parent)
:QObject(parent),d_ptr(new stkIGTLImageClientPrivate)
{
	Q_D(stkIGTLImageClient);

	d->imageClientSocket = igtl::ClientSocket::New();
	d->imageMessage = igtl::ImageMessage::New();

	d->imageSize[0] = 640;
	d->imageSize[1] = 480;
	d->imageSize[2] = 1;

	d->imageSpacing[0] = 1;
	d->imageSpacing[1] = 1;
	d->imageSpacing[2] = 5;

	d->imageOrigin[0] = 0;
	d->imageOrigin[1] = 0;
	d->imageOrigin[2] = 0;

	d->imageBufSize = 0;
}

//----------------------------------------------------------------------------
stkIGTLImageClient::~stkIGTLImageClient()
{
 
}

bool stkIGTLImageClient::isServerConnected()
{
	Q_D(stkIGTLImageClient);

	if( d->imageClientSocket.IsNull())
		return false;

	return d->imageClientSocket->GetConnected();
}

bool stkIGTLImageClient::ConnectIGTLServer(const char* hostname, int port)
{
	Q_D(stkIGTLImageClient);

	if(d->imageClientSocket.IsNull())
		return false;

	// return if the client is connected
	if(d->imageClientSocket->GetConnected())
		return true;

	if(d->imageClientSocket->ConnectToServer(hostname,port))
		return false;

	return true;
}

void stkIGTLImageClient::DisconnectIGTLServer()
{
	Q_D(stkIGTLImageClient);

	if(!d->imageClientSocket)
		return;

	if(d->imageClientSocket->GetConnected())
		d->imageClientSocket->CloseSocket();
}


void stkIGTLImageClient::SetImageSize(int x, int y, int z)
{
	Q_D(stkIGTLImageClient);

	int size[3];
	d->imageMessage->GetDimensions(size);

	if( size[0] == x && size[1] == y && size[2] == z)
		return;

	d->imageSize[0] = x;
	d->imageSize[1] = y;
	d->imageSize[2] = z;

	d->imageBufSize = d->imageMessage->GetScalarSize()*x*y*z;

	allocateImageMessage();
}

void stkIGTLImageClient::SetImageSpacing(float x, float y, float z)
{
	Q_D(stkIGTLImageClient);

	d->imageSpacing[0] = x;
	d->imageSpacing[1] = y;
	d->imageSpacing[2] = z;

	d->imageMessage->SetSpacing(d->imageSpacing);
}

void stkIGTLImageClient::SetImageOrigin(float x, float y, float z)
{
	Q_D(stkIGTLImageClient);

	d->imageOrigin[0] = x;
	d->imageOrigin[1] = y;
	d->imageOrigin[2] = z;

	d->imageMessage->SetOrigin(d->imageOrigin);
}

void stkIGTLImageClient::allocateImageMessage()
{
	Q_D(stkIGTLImageClient);

	d->imageMessage = igtl::ImageMessage::New();

	int   svoffset[] = {0, 0, 0}; // sub-volume offset

	igtl::Matrix4x4 matrix;
	matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;

	
	d->imageMessage->SetMatrix(matrix);
	d->imageMessage->SetDimensions(d->imageSize);
	d->imageMessage->SetSpacing(d->imageSpacing);
	d->imageMessage->SetOrigin(d->imageOrigin);
	d->imageMessage->SetNumComponents(1);
	d->imageMessage->SetScalarTypeToUint8();
	d->imageMessage->SetDeviceName("RTImage");
	d->imageMessage->SetSubVolume(d->imageSize, svoffset);

	d->imageBufSize =d->imageMessage->GetScalarSize()*d->imageSize[0]*d->imageSize[1]*d->imageSize[2];
	d->imageMessage->AllocateScalars();
}


bool stkIGTLImageClient::SendImage( unsigned char* bufPtr, int bufSize)
{
	Q_D(stkIGTLImageClient);

	if(d->imageBufSize < bufSize)
		return false;

	memcpy(d->imageMessage->GetScalarPointer(),bufPtr,bufSize);
	d->imageMessage->Pack();

	d->imageClientSocket->Send(d->imageMessage->GetPackPointer(), d->imageMessage->GetPackSize());

	return true;
}