// OpenIGTLinkIF MRML includes
#include "stkIGTLToMRMLImage.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// Slicer includes
//#include <vtkSlicerColorLogic.h>
#include <vtkMRMLColorLogic.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "qSlicerApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(stkIGTLToMRMLImage);
vtkCxxRevisionMacro(stkIGTLToMRMLImage, "$Revision: 15621 $");

//---------------------------------------------------------------------------
stkIGTLToMRMLImage::stkIGTLToMRMLImage()
{
}

//---------------------------------------------------------------------------
stkIGTLToMRMLImage::~stkIGTLToMRMLImage()
{
}

//---------------------------------------------------------------------------
void stkIGTLToMRMLImage::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* stkIGTLToMRMLImage::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLScalarVolumeDisplayNode *displayNode = NULL;
  vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
  vtkImageData* image = vtkImageData::New();

  //float fov = 256.0;
  image->SetDimensions(256, 256, 1);
  image->SetExtent(0, 255, 0, 255, 0, 0 );
  //image->SetSpacing( fov/256, fov/256, 10 );
  image->SetSpacing(1.0, 1.0, 1.0);
  //image->SetOrigin( fov/2, -fov/2, -0.0 );
  image->SetOrigin(0.0, 0.0, 0.0);
  image->SetNumberOfScalarComponents(1);
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();

  short* dest = (short*) image->GetScalarPointer();
  if (dest)
  {
	memset(dest, 0x00, 256*256*sizeof(unsigned char));
	image->Update();
  }

  scalarNode->SetAndObserveImageData(image);

  /* Based on the code in vtkSlicerVolumeLogic::AddHeaderVolume() */
  //displayNode = vtkMRMLVolumeDisplayNode::New();
  displayNode = vtkMRMLScalarVolumeDisplayNode::New();
  scalarNode->SetLabelMap(0);

  vtkMRMLNode* n = NULL;

  if (scalarNode != NULL)
    {
    scalarNode->SetName(name);
    scene->SaveStateForUndo();

    scalarNode->SetScene(scene);
    scalarNode->SetDescription("Received by OpenIGTLink");

    displayNode->SetScene(scene);


    /*double range[2];
    vtkDebugMacro("Set basic display info");
    scalarNode->GetImageData()->GetScalarRange(range);
    range[0] = 0.0;
    range[1] = 256.0;
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]) );*/

    vtkDebugMacro("Adding node..");
    scene->AddNode(displayNode);

	vtkMRMLColorLogic* colorLogic = vtkMRMLColorLogic::SafeDownCast(qSlicerApplication::application()->moduleManager()->module("Colors")->logic());
    if (colorLogic)
      {
      displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
      scalarNode->SetAndObserveDisplayNodeID(displayNode->GetID());
      }

    vtkDebugMacro("Name vol node "<<scalarNode->GetClassName());
    vtkDebugMacro("Display node "<<displayNode->GetClassName());

    n = scene->AddNode(scalarNode);
    vtkDebugMacro("Node added to scene");
    this->CenterImage(scalarNode);
    }

  scalarNode->Delete();
  displayNode->Delete();
  image->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* stkIGTLToMRMLImage::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent);

  return events;
}

//---------------------------------------------------------------------------
int stkIGTLToMRMLImage::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
	if (node == NULL)
		return 0;

	stkIGTLToMRMLBase::IGTLToMRML(buffer, node);

	if (strcmp(node->GetNodeTagName(), "Volume") != 0)
	{
		//std::cerr << "Invalid node!!!!" << std::endl;
		return 0;
	}

	// Create a message buffer to receive image data
	igtl::ImageMessage::Pointer imgMsg;
	imgMsg = igtl::ImageMessage::New();
	imgMsg->Copy(buffer); // !! TODO: copy makes performance issue.

	// Deserialize the transform data
	// If CheckCRC==0, CRC check is skipped.
	int c = imgMsg->Unpack(this->CheckCRC);

	if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
	{
		// TODO: error handling
		return 0;
	}

	// Retrive the image data
	int   size[3];          // image dimension
	float spacing[3];       // spacing (mm/pixel)
	float origin[3];
	int   svsize[3];        // sub-volume size
	int   svoffset[3];      // sub-volume offset
	int   scalarType;       // scalar type
	int   endian;
	igtl::Matrix4x4 matrix; // Image origin and orientation matrix

	scalarType = imgMsg->GetScalarType();
	endian = imgMsg->GetEndian();
	imgMsg->GetDimensions(size);
	imgMsg->GetSpacing(spacing);
	imgMsg->GetOrigin(origin);
	imgMsg->GetSubVolume(svsize, svoffset);
	imgMsg->GetMatrix(matrix);
	origin[1] -= 80;
	//origin[0] = -100;origin[1] = -100;origin[2] = 0;

	// check if the IGTL data fits to the current MRML node
	vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
	vtkImageData* newImageData = NULL;
	vtkImageData* imageData = volumeNode->GetImageData();
	int dsize[3];
	double dorigin[3];
	double dspacing[3];
	int dscalarType;
	imageData->GetDimensions(dsize);
	volumeNode->GetOrigin(dorigin);
	volumeNode->GetSpacing(dspacing);

	dscalarType = imageData->GetScalarType();
	if (dsize[0] != size[0] || dsize[1] != size[1] || dsize[2] != size[2] || scalarType != dscalarType 
		|| (origin[0] - dorigin[0])*(origin[0] - dorigin[0]) > 1  //原点变化
		|| (spacing[0] - dspacing[0])*(spacing[0] - dspacing[0]) > 0.1 ) //间距变化
	{
		newImageData = vtkImageData::New();
		newImageData->SetDimensions(size[0], size[1], size[2]);
		newImageData->SetExtent(0, size[0]-1, 0, size[1]-1, 0, size[2]-1);
		newImageData->SetOrigin(origin[0], origin[1], origin[2]);
		newImageData->SetSpacing(spacing[0], spacing[1], spacing[2]);
		newImageData->SetNumberOfScalarComponents(1);
		newImageData->SetScalarTypeToUnsignedChar(); // UINT8
		newImageData->AllocateScalars();
		imageData = newImageData;
	}

	memcpy(imageData->GetScalarPointer(),imgMsg->GetScalarPointer(), imgMsg->GetSubVolumeImageSize());


	if (newImageData)
	{
		volumeNode->SetAndObserveImageData(newImageData);
		newImageData->Delete();
	}

	imageData->Modified();
	volumeNode->Modified();

	return 1;
}

//---------------------------------------------------------------------------
int stkIGTLToMRMLImage::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  // If mrmlNode is Image node
  if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent && strcmp(mrmlNode->GetNodeTagName(), "Volume") == 0)
    {
    vtkMRMLScalarVolumeNode* volumeNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(mrmlNode);

    if (!volumeNode)
      {
      return 0;
      }

    vtkImageData* imageData = volumeNode->GetImageData();
    int   isize[3];          // image dimension
    //int   svsize[3];        // sub-volume size
    int   scalarType;       // scalar type
    double *origin;
    double *spacing;       // spacing (mm/pixel)
    int   ncomp;
    int   svoffset[] = {0, 0, 0};           // sub-volume offset

    scalarType = imageData->GetScalarType();
    ncomp = imageData->GetNumberOfScalarComponents();
    imageData->GetDimensions(isize);
    //imageData->GetExtent(0, isize[0]-1, 0, isize[1]-1, 0, isize[2]-1);
    origin = imageData->GetOrigin();
    spacing = imageData->GetSpacing();

    if (this->OutImageMessage.IsNull())
      {
      this->OutImageMessage = igtl::ImageMessage::New();
      }
    this->OutImageMessage->SetDimensions(isize);
    this->OutImageMessage->SetSpacing((float)spacing[0], (float)spacing[1], (float)spacing[2]);
    this->OutImageMessage->SetScalarType(scalarType);
    this->OutImageMessage->SetDeviceName(volumeNode->GetName());
    this->OutImageMessage->SetSubVolume(isize, svoffset);
    this->OutImageMessage->AllocateScalars();

    memcpy(this->OutImageMessage->GetScalarPointer(),
           imageData->GetScalarPointer(),
           this->OutImageMessage->GetImageSize());

    // Transform
    vtkMatrix4x4* rtimgTransform = vtkMatrix4x4::New();
    volumeNode->GetIJKToRASMatrix(rtimgTransform);
    float ntx = rtimgTransform->Element[0][0] / (float)spacing[0];
    float nty = rtimgTransform->Element[1][0] / (float)spacing[0];
    float ntz = rtimgTransform->Element[2][0] / (float)spacing[0];
    float nsx = rtimgTransform->Element[0][1] / (float)spacing[1];
    float nsy = rtimgTransform->Element[1][1] / (float)spacing[1];
    float nsz = rtimgTransform->Element[2][1] / (float)spacing[1];
    float nnx = rtimgTransform->Element[0][2] / (float)spacing[2];
    float nny = rtimgTransform->Element[1][2] / (float)spacing[2];
    float nnz = rtimgTransform->Element[2][2] / (float)spacing[2];
    float px  = rtimgTransform->Element[0][3];
    float py  = rtimgTransform->Element[1][3];
    float pz  = rtimgTransform->Element[2][3];

    rtimgTransform->Delete();

    // Shift the center
    // NOTE: The center of the image should be shifted due to different
    // definitions of image origin between VTK (Slicer) and OpenIGTLink;
    // OpenIGTLink image has its origin at the center, while VTK image
    // has one at the corner.

    float hfovi = (float)spacing[0] * (float)(isize[0]-1) / 2.0;
    float hfovj = (float)spacing[1] * (float)(isize[1]-1) / 2.0;
    float hfovk = (float)spacing[2] * (float)(isize[2]-1) / 2.0;

    float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
    float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
    float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;

    px = px + cx;
    py = py + cy;
    pz = pz + cz;

    igtl::Matrix4x4 matrix; // Image origin and orientation matrix
    matrix[0][0] = ntx;
    matrix[1][0] = nty;
    matrix[2][0] = ntz;
    matrix[0][1] = nsx;
    matrix[1][1] = nsy;
    matrix[2][1] = nsz;
    matrix[0][2] = nnx;
    matrix[1][2] = nny;
    matrix[2][2] = nnz;
    matrix[0][3] = px;
    matrix[1][3] = py;
    matrix[2][3] = pz;

    this->OutImageMessage->SetMatrix(matrix);
    this->OutImageMessage->Pack();

    *size = this->OutImageMessage->GetPackSize();
    *igtlMsg = (void*)this->OutImageMessage->GetPackPointer();

    return 1;
    }
  else
    {
    return 0;
    }

  return 0;
}

//---------------------------------------------------------------------------
void stkIGTLToMRMLImage::CenterImage(vtkMRMLVolumeNode *volumeNode)
{
    if ( volumeNode )
      {
      vtkImageData *image = volumeNode->GetImageData();
      if (image)
        {
        vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
        volumeNode->GetIJKToRASMatrix(ijkToRAS);

        double dimsH[4];
        double rasCorner[4];
        int *dims = image->GetDimensions();
        dimsH[0] = dims[0] - 1;
        dimsH[1] = dims[1] - 1;
        dimsH[2] = dims[2] - 1;
        dimsH[3] = 0.;
        ijkToRAS->MultiplyPoint(dimsH, rasCorner);

        double origin[3];
        int i;
        for (i = 0; i < 3; i++)
          {
          origin[i] = -0.5 * rasCorner[i];
          }
        volumeNode->SetDisableModifiedEvent(1);
        volumeNode->SetOrigin(origin);
        volumeNode->SetDisableModifiedEvent(0);
        volumeNode->InvokePendingModifiedEvent();

        ijkToRAS->Delete();
        }
      }
}
