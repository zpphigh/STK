// .NAME vtkMRMLProbeStorageNode - MRML node for representing a volume storage
// .SECTION Description
// vtkMRMLProbeStorageNode nodes describe the Annotation storage
// node that allows to read/write point data from/to file.

#ifndef __vtkMRMLProbeStorageNode_h
#define __vtkMRMLProbeStorageNode_h

#include "stkSlicerExtExport.h"
#include "vtkMRMLAnnotationLinesStorageNode.h"

class vtkMRMLProbeNode;

/// \ingroup Slicer_QtModules_Annotation
class  STK_SLICEREXT_EXPORT vtkMRMLProbeStorageNode : public vtkMRMLAnnotationLinesStorageNode
{
	/// Defined as a friend vtkMRMLAnnotationHierarchyStorageNode so that it can
	/// call the protected function WriteData(refNode, of)
	friend class vtkMRMLAnnotationHierarchyStorageNode;

public:
	static vtkMRMLProbeStorageNode *New();
	vtkTypeMacro(vtkMRMLProbeStorageNode,vtkMRMLAnnotationLinesStorageNode);
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual vtkMRMLNode* CreateNodeInstance();


	// Description:
	// Read node attributes from XML file
	virtual void ReadXMLAttributes( const char** atts);

	// Description:
	// Write this node's information to a MRML file in XML format.
	virtual void WriteXML(ostream& of, int indent);

	// Description:
	// Set dependencies between this node and the parent node
	// when parsing XML file
	virtual void ProcessParentNode(vtkMRMLNode *parentNode);

	// Description:
	// Copy the node's attributes to this object
	virtual void Copy(vtkMRMLNode *node);

	// Description:
	// Get node XML tag name (like Storage, Model)
	virtual const char* GetNodeTagName()  {return "AnnotationProbeStorage";};

	/// Read a single hyper plan Probe from an open list file, called by the hierarchy
	/// storage node
	int ReadOneProbe(fstream & fstr, vtkMRMLProbeNode *refNode);

	/// Return true if the node can be read in
	bool CanReadInReferenceNode(vtkMRMLNode* refNode);

protected:


	vtkMRMLProbeStorageNode();
	~vtkMRMLProbeStorageNode();
	vtkMRMLProbeStorageNode(const vtkMRMLProbeStorageNode&);
	void operator=(const vtkMRMLProbeStorageNode&);

	const char* GetAnnotationStorageType() { return "Probe"; }

	int WriteAnnotationProbeProperties(fstream & of, vtkMRMLProbeNode *refNode);
	int WriteAnnotationProbeData(fstream& of, vtkMRMLProbeNode *refNode);

	int ReadAnnotation(vtkMRMLProbeNode *refNode);
	int ReadAnnotationProbeData(vtkMRMLProbeNode *refNode, char line[1024], int typeColumn, int line1IDColumn, int selColumn,  int visColumn, int numColumns);
	int ReadAnnotationProbeProperties(vtkMRMLProbeNode *refNode, char line[1024], int &typeColumn, int& line1IDColumn, int& selColumn, int& visColumn, int& numColumns);

	/// Read data and set it in the referenced node
	virtual int ReadDataInternal(vtkMRMLNode *refNode);

	/// Write data from the referenced node into the stream
	virtual int WriteAnnotationDataInternal(vtkMRMLNode *refNode, fstream & of);

};

#endif
