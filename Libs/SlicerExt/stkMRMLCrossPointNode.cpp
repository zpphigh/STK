#include "stkMRMLCrossPointNode.h"

#include <sstream>
#include "vtkObjectFactory.h"


//----------------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(stkMRMLCrossPointNode);

//----------------------------------------------------------------------------------------
stkMRMLCrossPointNode::stkMRMLCrossPointNode()
{
	this->Point[0] = 0;
	this->Point[1] = 0;
	this->Point[2] = 0;
	this->Point[3] = 1;
}

//----------------------------------------------------------------------------------------
stkMRMLCrossPointNode::~stkMRMLCrossPointNode()
{
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::WriteXML(ostream& of, int indent)
{
	;
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::ReadXMLAttributes(const char** atts)
{
	;
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::Copy(vtkMRMLNode *node)
{
	Superclass::Copy(node);
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::UpdateScene(vtkMRMLScene *scene)
{
	Superclass::UpdateScene(scene);
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::ProcessMRMLEvents( vtkObject *caller,
											   unsigned long event, 
											   void *callData)
{
	Superclass::ProcessMRMLEvents(caller,event,callData);
}

//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::GetCrossPointCoordinates(double* point)
{
	point = this->Point;
}

//----------------------------------------------------------------------------------------
double* stkMRMLCrossPointNode::GetCrossPointCoordinates()
{
	return this->Point;
}
//----------------------------------------------------------------------------------------
void stkMRMLCrossPointNode::SetCrossPointCoordinates(double* p)
{
	this->Point[0] = p[0];
    this->Point[1] = p[1];
	this->Point[2] = p[2]; 
	this->Point[3] = p[3];
}
