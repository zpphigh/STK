#include <sstream>

#include "stkMRMLProbeNode.h"
#include "vtkMRMLAnnotationTextDisplayNode.h"
#include "vtkMRMLAnnotationPointDisplayNode.h"
#include "vtkMRMLAnnotationLineDisplayNode.h"
#include "stkMRMLProbeStorageNode.h"

#include <vtkAbstractTransform.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <qquaternion.h>
#include "qvector3d.h"


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLProbeNode);


//----------------------------------------------------------------------------
vtkMRMLProbeNode::vtkMRMLProbeNode()
{
	this->HideFromEditors = false;
	this->DistanceAnnotationFormat = NULL;
	this->SetDistanceAnnotationFormat("%.0f mm");
	this->Resolution = 5;
	this->ModelID1 = NULL;
	this->ModelID2 = NULL;

	this->Normal[0] = -1; // 针在横断面上投影为水平方向，并且针尾位于窗口右边
	this->Normal[1] = 0;
	this->Normal[2] = 0;

	/*this->Orientation->setVector(-1.0, 0.0, 0.0);*/  

	this->Length = 160.0;
	this->Power = 60;
	this->StartTime = 0;
	this->EndTime = 250;

	this->FocalLocked = false;
	this->VisibleIn2DWidget = true;
	this->VisibleIn3DWidget = true;
	this->Current = false;
	this->ColorChanged = false;

	this->Color[0] = 1.0;  // 默认颜色为白色
	this->Color[1] = 1.0;
	this->Color[2] = 1.0;
}

//----------------------------------------------------------------------------
vtkMRMLProbeNode::~vtkMRMLProbeNode()
{
	//vtkDebugMacro("Destructing...." << (this->GetID() != NULL ? this->GetID() : "null id"));
	if (this->DistanceAnnotationFormat)
	{
		delete [] this->DistanceAnnotationFormat;
		this->DistanceAnnotationFormat = NULL;
	}
	if (this->ModelID1)
	{
		delete [] this->ModelID1;
		this->ModelID1 = NULL;
	}
	if (this->ModelID2)
	{
		delete [] this->ModelID2;
		this->ModelID2 = NULL;
	}
}

void vtkMRMLProbeNode::WriteXML(ostream& of, int nIndent)
{
	Superclass::WriteXML(of, nIndent);

	vtkIndent indent(nIndent);

	of << indent << " ProbeDistanceAnnotationFormat=\"";
	if (this->DistanceAnnotationFormat) 
	{
		of << this->DistanceAnnotationFormat << "\"";
	}
	else 
	{
		of << "\"";
	}
	of << indent << " ProbeResolution=\""<< this->Resolution << "\"";

	if (this->ModelID1)
	{
		of << indent << " modelID1=\"" << this->ModelID1 << "\"";
	}
	if (this->ModelID2)
	{
		of << indent << " modelID2=\"" << this->ModelID2 << "\"";
	}
	if (this->distanceMeasurement)
	{
		of << indent << " distanceMeasurement=\"" << this->distanceMeasurement << "\"";
	}

}


//----------------------------------------------------------------------------
void vtkMRMLProbeNode::ReadXMLAttributes(const char** atts)
{
	// cout << "vtkMRMLProbeNode::ReadXMLAttributes start"<< endl;

	int disabledModify = this->StartModify();

	this->ResetAnnotations();

	Superclass::ReadXMLAttributes(atts);


	while (*atts != NULL) 
	{
		const char* attName = *(atts++);
		std::string attValue(*(atts++));


		if (!strcmp(attName, "ProbeResolution"))       
		{

			std::stringstream ss;
			ss << attValue;
			ss >> this->Resolution;
		}
		else if (!strcmp(attName, "ProbeDistanceAnnotationFormat"))
		{
			this->SetDistanceAnnotationFormat(attValue.c_str());
		}

		else if (!strcmp(attName, "modelID1"))
		{
			this->SetModelID1(attValue.c_str());
		}
		else if (!strcmp(attName, "modelID2"))
		{
			this->SetModelID2(attValue.c_str());
		}
		else if (!strcmp(attName, "distanceMeasurement"))
		{
			std::stringstream ss;
			ss << attValue;
			ss >> this->distanceMeasurement;
		}
	}
	this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLProbeNode::Copy(vtkMRMLNode *anode)
{

	Superclass::Copy(anode);
	vtkMRMLProbeNode *node = (vtkMRMLProbeNode *) anode;

	this->SetDistanceMeasurement(node->GetDistanceMeasurement());
}

//-----------------------------------------------------------
void vtkMRMLProbeNode::UpdateScene(vtkMRMLScene *scene)
{
	Superclass::UpdateScene(scene);

	// Nothing to do at this point  bc vtkMRMLAnnotationDisplayNode is subclass of vtkMRMLModelDisplayNode 
	// => will be taken care of by vtkMRMLModelDisplayNode  

}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::ProcessMRMLEvents ( vtkObject *caller,
	unsigned long event, 
	void *callData )
{
	Superclass::ProcessMRMLEvents(caller, event, callData);

	// Not necessary bc vtkMRMLAnnotationDisplayNode is subclass of vtkMRMLModelDisplayNode 
	// => will be taken care of  in vtkMRMLModelNode
}

//----------------------------------------------------------------------------
void vtkMRMLProbeNode::PrintAnnotationInfo(ostream& os, vtkIndent indent, int titleFlag)
{
	//cout << "vtkMRMLProbeNode::PrintAnnotationInfo" << endl;
	if (titleFlag) 
	{

		os <<indent << "vtkMRMLProbeNode: Annotation Summary";
		if (this->GetName()) 
		{
			os << " of " << this->GetName();
		}
		os << endl;
	}

	Superclass::PrintAnnotationInfo(os, indent, 0);

	os << indent << "ProbeDistanceAnnotationFormat: ";
	if (this->DistanceAnnotationFormat) 
	{
		os  << this->DistanceAnnotationFormat << "\n";
	}
	else 
	{
		os  << "(None)" << "\n";
	}
	os << indent << "ProbeResolution: " << this->Resolution << "\n";

	os << indent << "Model 1: " << (this->ModelID1 ? this->ModelID1 : "none") << "\n";
	os << indent << "Model 2: " << (this->ModelID2 ? this->ModelID2 : "none") << "\n";

}

//---------------------------------------------------------------------------
int vtkMRMLProbeNode::AddControlPoint(double newControl[3],int selectedFlag, int visibleFlag)
{
	if (this->GetNumberOfControlPoints() > 1) {
		vtkErrorMacro("AnnotationProbe: "<< this->GetName() << " cannot have more than 3 control points !");
		return -1;
	}
	return Superclass::AddControlPoint(newControl,selectedFlag,visibleFlag);
}

//---------------------------------------------------------------------------
double vtkMRMLProbeNode::GetDistanceAnnotationScale()
{
	vtkMRMLAnnotationTextDisplayNode *node = this->GetAnnotationTextDisplayNode();
	if (!node)
	{
		return 0;
	}
	return node->GetTextScale();
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetDistanceAnnotationScale(double init)
{
	vtkMRMLAnnotationTextDisplayNode *node = this->GetAnnotationTextDisplayNode();

	if (!node)
	{
		vtkErrorMacro("AnnotationProbe: "<< this->GetName() << " cannot get AnnotationTextDisplayNode");
		return;
	}
	node->SetTextScale(init);
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetDistanceAnnotationVisibility(int flag)
{
	this->SetAnnotationAttribute(0,vtkMRMLAnnotationNode::TEXT_VISIBLE,flag);

}

//---------------------------------------------------------------------------
int vtkMRMLProbeNode::GetDistanceAnnotationVisibility()
{
	return this->GetAnnotationAttribute(0, vtkMRMLAnnotationNode::TEXT_VISIBLE);
}

int vtkMRMLProbeNode::SetProbe(vtkIdType line1Id, int sel, int vis)
{
	vtkIdType line1IDPoints[2];
	this->GetEndPointsId(line1Id,line1IDPoints);

	//Change this later
	if (line1IDPoints[0]!= 0 || line1IDPoints[1] != 1)
	{
		vtkErrorMacro("Not valid line definition!");
		return -1;
	}
	this->SetSelected(sel); 
	this->SetVisible(vis); 

	return 1;
}

//---------------------------------------------------------------------------
int vtkMRMLProbeNode::SetControlPoint(int id, double newControl[3])
{
	if (id < 0 || id > 1) {
		return 0;
	}

	int flag = Superclass::SetControlPoint(id, newControl,1,1);
	if (!flag) 
	{
		return 0;
	}
	if (this->GetNumberOfControlPoints() < 2) 
	{
		return 1;
	}

	this->AddLine(0,1,1,1);
	return 1;
}

//---------------------------------------------------------------------------
double* vtkMRMLProbeNode::GetPointColour()
{
	vtkMRMLAnnotationPointDisplayNode *node = this->GetAnnotationPointDisplayNode();
	if (!node)
	{
		return 0;
	}
	return node->GetSelectedColor();
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetPointColour(double initColor[3])
{
	vtkMRMLAnnotationPointDisplayNode *node = this->GetAnnotationPointDisplayNode();
	if (!node)
	{
		vtkErrorMacro("AnnotationProbe: "<< this->GetName() << " cannot get AnnotationPointDisplayNode");
		return;
	}
	node->SetSelectedColor(initColor);
}

//---------------------------------------------------------------------------
double* vtkMRMLProbeNode::GetDistanceAnnotationTextColour()
{
	vtkMRMLAnnotationTextDisplayNode *node = this->GetAnnotationTextDisplayNode();
	if (!node)
	{
		return 0;
	}
	return node->GetSelectedColor();
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetDistanceAnnotationTextColour(double initColor[3])
{
	vtkMRMLAnnotationTextDisplayNode *node = this->GetAnnotationTextDisplayNode();
	if (!node)
	{
		vtkErrorMacro("AnnotationProbe: "<< this->GetName() << " cannot get AnnotationPointDisplayNode");
		return;
	}
	node->SetSelectedColor(initColor);
}

//---------------------------------------------------------------------------
double* vtkMRMLProbeNode::GetLineColour()
{
	vtkMRMLAnnotationLineDisplayNode *node = this->GetAnnotationLineDisplayNode();
	if (!node)
	{
		return 0;
	}
	return node->GetSelectedColor();
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetLineColour(double initColor[3])
{
	vtkMRMLAnnotationLineDisplayNode *node = this->GetAnnotationLineDisplayNode();
	if (!node)
	{
		vtkErrorMacro("AnnotationProbe: "<< this->GetName() << " cannot get AnnotationPointDisplayNode");
		return;
	}
	node->SetSelectedColor(initColor);
}

void vtkMRMLProbeNode::ApplyTransformMatrix(vtkMatrix4x4* transformMatrix)
{
	double (*matrix)[4] = transformMatrix->Element;
	double xyzIn[3];
	double xyzOut[3];
	double *p = NULL;

	// first point
	/*p = this->GetEndpoint();
	if (p)
	{
		xyzIn[0] = p[0];
		xyzIn[1] = p[1];
		xyzIn[2] = p[2];

		xyzOut[0] = matrix[0][0]*xyzIn[0] + matrix[0][1]*xyzIn[1] + matrix[0][2]*xyzIn[2] + matrix[0][3];
		xyzOut[1] = matrix[1][0]*xyzIn[0] + matrix[1][1]*xyzIn[1] + matrix[1][2]*xyzIn[2] + matrix[1][3];
		xyzOut[2] = matrix[2][0]*xyzIn[0] + matrix[2][1]*xyzIn[1] + matrix[2][2]*xyzIn[2] + matrix[2][3];
		this->SetPosition1(xyzOut);
	}*/

	// second point
	p = this->GetHeader();
	if (p)
	{
		xyzIn[0] = p[0];
		xyzIn[1] = p[1];
		xyzIn[2] = p[2];

		xyzOut[0] = matrix[0][0]*xyzIn[0] + matrix[0][1]*xyzIn[1] + matrix[0][2]*xyzIn[2] + matrix[0][3];
		xyzOut[1] = matrix[1][0]*xyzIn[0] + matrix[1][1]*xyzIn[1] + matrix[1][2]*xyzIn[2] + matrix[1][3];
		xyzOut[2] = matrix[2][0]*xyzIn[0] + matrix[2][1]*xyzIn[1] + matrix[2][2]*xyzIn[2] + matrix[2][3];
		this->SetPosition(xyzOut);
	}
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::ApplyTransform(vtkAbstractTransform* transform)
{
	double xyzIn[3];
	double xyzOut[3];
	double *p;

	// first point
	/*p = this->GetHeader1();
	if (p)
	{
		xyzIn[0] = p[0];
		xyzIn[1] = p[1];
		xyzIn[2] = p[2];

		transform->TransformPoint(xyzIn,xyzOut);
		this->SetPosition1(xyzOut);
	}*/

	// second point
	p = this->GetHeader();
	if (p)
	{
		xyzIn[0] = p[0];
		xyzIn[1] = p[1];
		xyzIn[2] = p[2];

		transform->TransformPoint(xyzIn,xyzOut);
		this->SetPosition(xyzOut);
	}
}

//---------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLProbeNode::CreateDefaultStorageNode()
{
	return vtkMRMLStorageNode::SafeDownCast(vtkMRMLProbeStorageNode::New());
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::SetDistanceMeasurement(double val)
{
	this->distanceMeasurement = val;
	this->InvokeEvent(vtkMRMLProbeNode::ValueModifiedEvent);
}

//---------------------------------------------------------------------------
double vtkMRMLProbeNode::GetDistanceMeasurement()
{
	return this->distanceMeasurement;
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::GetHeader(double* position)
{
	double * tmp = this->GetHeader();
	if (!tmp)
	{
		vtkErrorMacro("AnnotationProbe - Failed to get Header coordinate");
		return;
	}
	for(int i=0; i < 3; ++i)
	{ 
		position[i] = tmp[i]; 
	}
}

//---------------------------------------------------------------------------
void vtkMRMLProbeNode::GetTail(double* position)
{
	double length = 27;
	double header[4] = {0, 0, 0, 1};
	this->GetHeader(header);

	double norm[3] = {0, 0, 0};
	this->GetNormal(norm);

	position[0] = header[0] + norm[0] * length;
	position[1] = header[1] + norm[1] * length;
	position[2] = header[2] + norm[2] * length;
	position[3] = 1;
}

//---------------------------------------------------------------------------
//void vtkMRMLProbeNode::GetEndpoint(double* position)
//{
//	double * tmp = this->GetEndpoint();
//	if (!tmp)
//	{
//		vtkErrorMacro("AnnotationProbe - Failed to get second control point");
//		return;
//	}
//	for(int i=0; i < 3; ++i){ position[i] = tmp[i]; }
//}
