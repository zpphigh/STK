// OpenIGTLinkIF MRML includes
#include "stkIGTLToMRMLPosition.h"

// OpenIGTLink includes
#include <igtlPositionMessage.h>
#include <igtlWin32Header.h>
#include <igtlMath.h>

// MRML includes
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLModelDisplayNode.h>
//#include "vtkMRMLProbeNode.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkAppendPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSTLReader.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>



//---------------------------------------------------------------------------
vtkStandardNewMacro(stkIGTLToMRMLPosition);
vtkCxxRevisionMacro(stkIGTLToMRMLPosition, "$Revision: 15552 $");

//---------------------------------------------------------------------------
stkIGTLToMRMLPosition::stkIGTLToMRMLPosition()
{
}

//---------------------------------------------------------------------------
stkIGTLToMRMLPosition::~stkIGTLToMRMLPosition()
{
}

//---------------------------------------------------------------------------
void stkIGTLToMRMLPosition::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* stkIGTLToMRMLPosition::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLLinearTransformNode* transformNode;

  transformNode = vtkMRMLLinearTransformNode::New();
  transformNode->SetName(name);
  transformNode->SetDescription("Received by OpenIGTLink");

  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  transform->Identity();
  //transformNode->SetAndObserveImageData(transform);
  transformNode->ApplyTransformMatrix(transform);
  transform->Delete();

  vtkMRMLNode* n = scene->AddNode(transformNode);
  this->SetVisibility(1,scene,transformNode);
  transformNode->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* stkIGTLToMRMLPosition::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);

  return events;
}

//---------------------------------------------------------------------------
int stkIGTLToMRMLPosition::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  stkIGTLToMRMLBase::IGTLToMRML(buffer, node);

  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->Copy(buffer);  // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = positionMsg->Unpack(this->CheckCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  if (node == NULL)
    {
    return 0;
    }

  vtkMRMLLinearTransformNode* transformNode =
    vtkMRMLLinearTransformNode::SafeDownCast(node);

  float position[3];
  float quaternion[4];
  igtl::Matrix4x4 matrix;
  positionMsg->GetPosition(position);
  positionMsg->GetQuaternion(quaternion);

  igtl::QuaternionToMatrix(quaternion, matrix);

  vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
  int row, column;
  for (row = 0; row < 3; row++)
    {
    for (column = 0; column < 3; column++)
      {
      transformToParent->Element[row][column] = matrix[row][column];
      }
    transformToParent->Element[row][3] = position[row];
    }
  transformToParent->Element[3][0] = 0.0;
  transformToParent->Element[3][1] = 0.0;
  transformToParent->Element[3][2] = 0.0;
  transformToParent->Element[3][3] = 1.0;

  transformToParent->Modified();

  return 1;
}

//---------------------------------------------------------------------------
int stkIGTLToMRMLPosition::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (mrmlNode && event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {
    vtkMRMLLinearTransformNode* transformNode =
      vtkMRMLLinearTransformNode::SafeDownCast(mrmlNode);
    vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();

    //igtl::PositionMessage::Pointer OutPositionMsg;
    if (this->OutPositionMsg.IsNull())
      {
      this->OutPositionMsg = igtl::PositionMessage::New();
      }

    this->OutPositionMsg->SetDeviceName(mrmlNode->GetName());

    igtl::Matrix4x4 igtlmatrix;

    igtlmatrix[0][0]  = matrix->Element[0][0];
    igtlmatrix[1][0]  = matrix->Element[1][0];
    igtlmatrix[2][0]  = matrix->Element[2][0];
    igtlmatrix[3][0]  = matrix->Element[3][0];
    igtlmatrix[0][1]  = matrix->Element[0][1];
    igtlmatrix[1][1]  = matrix->Element[1][1];
    igtlmatrix[2][1]  = matrix->Element[2][1];
    igtlmatrix[3][1]  = matrix->Element[3][1];
    igtlmatrix[0][2]  = matrix->Element[0][2];
    igtlmatrix[1][2]  = matrix->Element[1][2];
    igtlmatrix[2][2]  = matrix->Element[2][2];
    igtlmatrix[3][2]  = matrix->Element[3][2];
    igtlmatrix[0][3]  = matrix->Element[0][3];
    igtlmatrix[1][3]  = matrix->Element[1][3];
    igtlmatrix[2][3]  = matrix->Element[2][3];
    igtlmatrix[3][3]  = matrix->Element[3][3];

    float position[3];
    float quaternion[4];

    position[0] = igtlmatrix[0][3];
    position[1] = igtlmatrix[1][3];
    position[2] = igtlmatrix[2][3];
    igtl::MatrixToQuaternion(igtlmatrix, quaternion);

    //this->OutPositionMsg->SetMatrix(igtlmatrix);
    this->OutPositionMsg->SetPosition(position);
    this->OutPositionMsg->SetQuaternion(quaternion);
    this->OutPositionMsg->Pack();

    *size = this->OutPositionMsg->GetPackSize();
    *igtlMsg = (void*)this->OutPositionMsg->GetPackPointer();

    return 1;
    }

  return 0;
}

//---------------------------------------------------------------------------
void stkIGTLToMRMLPosition::SetVisibility(int sw, vtkMRMLScene * scene, vtkMRMLNode * node)
{
	vtkMRMLLinearTransformNode * tnode = vtkMRMLLinearTransformNode::SafeDownCast(node);

	if (!tnode || !scene)
	{
		// If the node is not a linear transform node, do nothing.
		return;
	}

	vtkMRMLModelNode*   locatorModel = NULL;
	vtkMRMLDisplayNode* locatorDisp  = NULL;

	const char * attr = tnode->GetAttribute("IGTLModelID");
	if (!attr || !scene->GetNodeByID(attr)) // no locator has been created
	{
		if (sw)
		{
			std::stringstream ss;
			ss << "Locator_" << tnode->GetName();
			locatorModel = AddLocatorModel(scene, ss.str().c_str(), 0.0, 1.0, 1.0);
			if (locatorModel)
			{
				tnode->SetAttribute("IGTLModelID", locatorModel->GetID());
				scene->Modified();
				locatorModel->SetAndObserveTransformNodeID(tnode->GetID());
				locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
			}
		}
		else
		{
			locatorModel = NULL;
		}
	}
	else
	{
		locatorModel = vtkMRMLModelNode::SafeDownCast(scene->GetNodeByID(attr));
	}
	if (locatorModel)
	{
		locatorDisp = locatorModel->GetDisplayNode();
		locatorDisp->SetVisibility(sw);
		locatorModel->Modified();
	}
}



vtkMRMLModelNode* stkIGTLToMRMLPosition::AddLocatorModel(vtkMRMLScene * scene, const char* nodeName, double r, double g, double b)
{
	vtkMRMLModelNode           *locatorModel;
	vtkMRMLModelDisplayNode    *locatorDisp;

	locatorModel = vtkMRMLModelNode::New();
	locatorDisp = vtkMRMLModelDisplayNode::New();

	// Cylinder represents the locator stick
	vtkCylinderSource *cylinder = vtkCylinderSource::New();
	cylinder->SetRadius(1.5);
	cylinder->SetHeight(100);
	cylinder->SetCenter(0, 0, 0);
	cylinder->Update();

	// Rotate cylinder
	char* str1 = "CalibrationTool";
	char* str2 = "UltrasoundTool";
	char* str3 = "SurgeryTool";
	vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
	vtkTransform* trans =   vtkTransform::New();
	if(strstr(nodeName,str1) != NULL)           //CalibrationTool
	{
		//trans->RotateZ(-90.0);                            //polaris,acession的坐标系
		trans->RotateX(90.0);                             //aurora的坐标系
	}else if(strstr(nodeName,str2) != NULL)           //UltrasoundTool
	{
		trans->RotateZ(90.0);                             //aurora的坐标系
	}else if(strstr(nodeName,str3) != NULL)           //SurgeryTool
	{
		trans->RotateX(90.0);                             //aurora的坐标系
	}
	trans->Translate(0.0, -50.0, 0.0);
	
	trans->Update();
	tfilter->SetInput(cylinder->GetOutput());
	tfilter->SetTransform(trans);
	tfilter->Update();

	// Sphere represents the locator tip
	vtkSphereSource *sphere = vtkSphereSource::New();
	sphere->SetRadius(3.0);
	sphere->SetCenter(0, 0, 0);
	sphere->Update();

	vtkAppendPolyData *apd = vtkAppendPolyData::New();
	apd->AddInput(sphere->GetOutput());
	//apd->AddInput(cylinder->GetOutput());
	apd->AddInput(tfilter->GetOutput());

	//if( std::string(nodeName) == "Locator_CalibrationTool")
	//{
	//	vtkSTLReader* reader = vtkSTLReader::New();
	// 	reader->SetFileName("C:/config/MicrowaveTool.stl");
	//	reader->Update();
	//	apd->AddInput(reader->GetOutput());
	//	reader->Delete();
	//}
	//else if ( std::string(nodeName) == "Locator_MicrowaveTool")
	//{
	//	vtkSTLReader* reader = vtkSTLReader::New();
	//	reader->SetFileName("C:/config/MicrowaveTool.stl");
	//	reader->Update();
	//	apd->AddInput(reader->GetOutput());
	//	reader->Delete();
	//}
	


	apd->Update();

	locatorModel->SetAndObservePolyData(apd->GetOutput());

	double color[3];
	color[0] = r;
	color[1] = g;
	color[2] = b;
//	locatorDisp->SetPolyData(locatorModel->GetPolyData());
	locatorDisp->SetColor(color);

	trans->Delete();
	tfilter->Delete();
	cylinder->Delete();
	sphere->Delete();
	apd->Delete();

	scene->SaveStateForUndo();
	scene->AddNode(locatorDisp);
	vtkMRMLNode* lm = scene->AddNode(locatorModel);
	locatorDisp->SetScene(scene);
	locatorModel->SetName(nodeName);
	locatorModel->SetScene(scene);
	locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
	locatorModel->SetHideFromEditors(0);

	locatorModel->Delete();
	locatorDisp->Delete();

	return vtkMRMLModelNode::SafeDownCast(lm);
}
