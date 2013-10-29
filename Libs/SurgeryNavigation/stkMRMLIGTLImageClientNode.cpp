// OpenIGTLinkIF MRML includes
#include "stkMRMLIGTLImageClientNode.h"

// OpenIGTLink includes
#include <igtlServerSocket.h>

#include <igtlOSUtil.h>
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>

// MRML includes
#include <vtkMRMLScene.h>
#include "vtkObjectFactory.h"


//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(stkMRMLIGTLImageClientNode);


//----------------------------------------------------------------------------
stkMRMLIGTLImageClientNode::stkMRMLIGTLImageClientNode()
{
	imageClientSocket = igtl::ClientSocket::New();

	imageSize[0] = 0;
	imageSize[1] = 0;
	imageSize[2] = 0;

	imageSpacing[0] = 0;
	imageSpacing[1] = 0;
	imageSpacing[2] = 0;

	imageOrigin[0] = 0;
	imageOrigin[1] = 0;
	imageOrigin[2] = 0;
 
}

//----------------------------------------------------------------------------
stkMRMLIGTLImageClientNode::~stkMRMLIGTLImageClientNode()
{
 

}

void stkMRMLIGTLImageClientNode::SetImageSize(int x, int y, int z)
{
	imageSize[0] = x;
	imageSize[1] = y;
	imageSize[2] = z;
}

void stkMRMLIGTLImageClientNode::SetImageSpacing(float x, float y, float z)
{
	imageSpacing[0] = x;
	imageSpacing[1] = y;
	imageSpacing[2] = z;
}

void stkMRMLIGTLImageClientNode::SetImageOrigin(float x, float y, float z)
{
	imageOrigin[0] = x;
	imageOrigin[1] = y;
	imageOrigin[2] = z;
}

void stkMRMLIGTLImageClientNode::allocateImageMessage()
{
	imageMessage = igtl::ImageMessage::New();

	int   svoffset[] = {0, 0, 0}; // sub-volume offset
	int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type

	igtl::Matrix4x4 matrix;
	matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;

	imageMessage->SetMatrix(matrix);
	imageMessage->SetDimensions(imageSize);
	imageMessage->SetSpacing(imageSpacing);
	imageMessage->SetOrigin(imageOrigin);
	imageMessage->SetScalarType(scalarType);
	imageMessage->SetDeviceName("ImageMessage");
	imageMessage->SetSubVolume(imageSize, svoffset);
	imageMessage->AllocateScalars();
}

