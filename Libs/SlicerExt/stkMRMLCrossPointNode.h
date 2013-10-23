/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"

// MRML
#include "vtkMRMLNode.h"

/// \ingroup Slicer_QtModules_Annotation
class  STK_SLICEREXT_EXPORT stkMRMLCrossPointNode: public vtkMRMLNode
{
public:
	static stkMRMLCrossPointNode *New();
	vtkTypeMacro(stkMRMLCrossPointNode,vtkMRMLNode);
	// Description:
	// Just prints short summary 
	void PrintAnnotationInfo(ostream& os, vtkIndent indent, int titleFlag = 1);

	//--------------------------------------------------------------------------
	// MRMLNode methods
	//--------------------------------------------------------------------------

	virtual vtkMRMLNode* CreateNodeInstance();
	// Description:
	// Get node XML tag name (like Volume, Model)
	virtual const char* GetNodeTagName() {return "CrossPoint";}

	virtual const char* GetIcon() {return " ";}

	// Description:
	// Read node attributes from XML file
	virtual void ReadXMLAttributes( const char** atts);

	// Description:
	// Write this node's information to a MRML file in XML format.
	virtual void WriteXML(ostream& of, int indent);

	// Description:
	// Copy the node's attributes to this object
	virtual void Copy(vtkMRMLNode *node);

	void UpdateScene(vtkMRMLScene *scene);

	// Description:
	// alternative method to propagate events generated in Display nodes
	virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
		unsigned long /*event*/, 
		void * /*callData*/ );

	double* GetCrossPointCoordinates();
	void GetCrossPointCoordinates(double* point);
	void SetCrossPointCoordinates(double* p);

protected:
	stkMRMLCrossPointNode();
	virtual ~stkMRMLCrossPointNode();
	stkMRMLCrossPointNode(const stkMRMLCrossPointNode&);
	void operator=(const stkMRMLCrossPointNode&);

private:
	double Point[4];
};