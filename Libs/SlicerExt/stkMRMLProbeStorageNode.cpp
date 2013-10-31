
#include "vtkObjectFactory.h"
#include "stkMRMLProbeStorageNode.h"
#include "stkMRMLProbeNode.h"
//#include "vtkMRMLHierarchyNode.h"
#include "vtkMRMLScene.h"
#include "vtkStringArray.h"

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLProbeStorageNode);

//----------------------------------------------------------------------------
vtkMRMLProbeStorageNode::vtkMRMLProbeStorageNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLProbeStorageNode::~vtkMRMLProbeStorageNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLProbeStorageNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLProbeStorageNode::ReadXMLAttributes(const char** atts)
{

  Superclass::ReadXMLAttributes(atts);

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void vtkMRMLProbeStorageNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
}

//----------------------------------------------------------------------------
void vtkMRMLProbeStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{  
  vtkMRMLStorageNode::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLProbeStorageNode::ProcessParentNode(vtkMRMLNode *parentNode)
{
  this->ReadData(parentNode);
}

 //----------------------------------------------------------------------------
int vtkMRMLProbeStorageNode::ReadAnnotationProbeData(vtkMRMLProbeNode *refNode, char line[1024],
                                   int typeColumn, int line1IDColumn, int selColumn,  int visColumn, int numColumns)
{
  if (!refNode)
    {
      return -1;
    }

  if (typeColumn)
    {
      vtkErrorMacro("Type column has to be zero !");
      return -1;
    }

  // is it empty?

  if (line[0] == '\0')
    {
      //vtkDebugMacro("Empty line, skipping:\n\"" << line << "\"");
      return 1;
    }

  //vtkDebugMacro("got a line: \n\"" << line << "\""); 

  std::string attValue(line);
  size_t size = std::string(this->GetAnnotationStorageType()).size();
 
  if (attValue.compare(0,size,this->GetAnnotationStorageType()))
    {
      return 0;
    }

  int sel = 1, vis = 1;
  std::string Annotation;
  
  // Jump over type 
  size_t  startPos =attValue.find("|",0) +1;
  size_t  endPos =attValue.find("|",startPos);
  int columnNumber = 1;
  vtkIdType lineID = -1;
  while (startPos != std::string::npos && (columnNumber < numColumns)) 
    {
    if (startPos != endPos) 
      {
      std::string tokenString;
      if (endPos == std::string::npos) 
        {
        tokenString = attValue.substr(startPos,endPos);
        }
      else
        {
        tokenString = attValue.substr(startPos,endPos-startPos);
        }
      
      if (columnNumber == line1IDColumn)
        {
        lineID = atoi(tokenString.c_str());
        }
     else if (columnNumber == selColumn)
        {
        sel = atoi(tokenString.c_str());
        }
      else if (columnNumber == visColumn)
        {
        vis = atoi(tokenString.c_str());
        }
      }
    startPos = endPos +1;
    endPos =attValue.find("|",startPos);
    columnNumber ++;
    }

  if (refNode->SetProbe(lineID, sel, vis) < 0 ) 
    {
    vtkErrorMacro("Error setting angle , lineID = " << lineID);
    return -1;
    }

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLProbeStorageNode::ReadAnnotationProbeProperties(vtkMRMLProbeNode *refNode, char line[1024], int &typeColumn, 
                                     int& line1IDColumn, int& selColumn, int& visColumn, int& numColumns)
{
  // cout << "vtkMRMLProbeStorageNode::ReadAnnotationProbeProperties " << line << endl;
 if (line[0] != '#' || line[1] != ' ') 
    {
      return 0;
    }
 //vtkDebugMacro("Comment line, checking:\n\"" << line << "\"");

 std::string preposition = std::string("# ") + this->GetAnnotationStorageType();
 size_t pointOffset = preposition.size();

 //this->DebugOn(); 
 //vtkDebugMacro("Have a possible option in line " << line);
 std::string lineString = std::string(line);
 
 if (lineString.find(preposition + "Resolution = ") != std::string::npos)
   {
     std::string str = lineString.substr(13 + pointOffset,std::string::npos);
     //vtkDebugMacro("Getting Resolution, substr = " << str);
     float size = atof(str.c_str());
     refNode->SetResolution(size);
     //this->DebugOff();

     return 1;
   }
 else if (lineString.find(preposition + "DistanceAnnotationFormat = ") != std::string::npos)
   {
     std::string str = lineString.substr(27 + pointOffset,std::string::npos);
     //vtkDebugMacro("Getting DistanceAnnotationFormat, substr = " << str);
     refNode->SetDistanceAnnotationFormat(str.c_str());
     //this->DebugOff();
     return 1;
   } 
 else if (lineString.find(preposition + "Columns = ") != std::string::npos)
    {
      std::string str = lineString.substr(10 + pointOffset, std::string::npos);
      
      //vtkDebugMacro("Getting column order for the fids, substr = " << str.c_str());
      // reset all of them
      typeColumn= line1IDColumn = selColumn = visColumn = -1;
      numColumns = 0;
      char *columns = (char *)str.c_str();
      char *ptr = strtok(columns, "|");
      while (ptr != NULL)
    {
      if (strcmp(ptr, "type") == 0)
        {
          typeColumn = numColumns ;
        }
      else if (strcmp(ptr, "line1ID") == 0)
        {
          line1IDColumn =  numColumns;
        }
      else if (strcmp(ptr, "sel") == 0)
        {
          selColumn =  numColumns;
        }
      else if (strcmp(ptr, "vis" ) == 0)
        {
          visColumn =  numColumns;
        }
      ptr = strtok(NULL, "|");
      numColumns++;
    }
      // set the total number of columns
      //vtkDebugMacro("Got " << numColumns << " columns, type = " << typeColumn << ", line1ID = " << line1IDColumn << ", sel = " <<  selColumn << ", vis = " << visColumn);
      //this->DebugOff();
      return 1;
    }
 //this->DebugOff();
  return 0;
}


//----------------------------------------------------------------------------
// assumes that the node is already reset
int vtkMRMLProbeStorageNode::ReadAnnotation(vtkMRMLProbeNode *refNode)
{

  if (refNode == NULL)
    {
    vtkErrorMacro("ReadAnnotation: unable to cast input node " << refNode->GetID() << " to a Annotation node");
    return 0;
    }

  if (!Superclass::ReadAnnotation(refNode)) 
    {
    return 0;
    }

  // open the file for reading input
  fstream fstr;
  if (!this->OpenFileToRead(fstr, refNode))
    {
    return 0;
    }

  // turn off modified events
  int modFlag = refNode->GetDisableModifiedEvent();
  refNode->DisableModifiedEventOn();
  char line[1024];
  // default column ordering for Annotation info - this is exactly the same as for fiducial
  // first pass: line will have label,x,y,z,selected,visible
  int typePointColumn = 0;
  int line1IDColumn = 1;
  int selPointColumn  = 4;
  int visPointColumn  = 5;
  int numPointColumns = 6;

  while (fstr.good())
    {
    fstr.getline(line, 1024);
    
    // does it start with a #?
        // Property
    if ((line[0] == '#') && (line[1] == ' ')) 
      {
        this->ReadAnnotationProbeProperties(refNode, line, typePointColumn, line1IDColumn, selPointColumn, visPointColumn, numPointColumns); 
      }
        else
          {
        if (this->ReadAnnotationProbeData(refNode, line, typePointColumn, line1IDColumn, selPointColumn,  
                          visPointColumn, numPointColumns) < 0 ) 
          {
        return 0;
          }
      }
    }   
    refNode->SetDisableModifiedEvent(modFlag);

    fstr.close();

    return 1;

}

//----------------------------------------------------------------------------
bool vtkMRMLProbeStorageNode::CanReadInReferenceNode(vtkMRMLNode* refNode)
{
	return refNode->IsA("vtkMRMLAnnotationRulerNode");
}

//----------------------------------------------------------------------------
int vtkMRMLProbeStorageNode::ReadDataInternal(vtkMRMLNode *refNode)
{
	// cast the input node
	vtkMRMLProbeNode *aNode =
		vtkMRMLProbeNode::SafeDownCast(refNode);

	if (aNode == NULL)
	{
		vtkErrorMacro("ReadData: unable to cast input node " << refNode->GetID() << " to a annotation control point node");
		return 0;
	}

	// clear out the list
	aNode->ResetAnnotations();

	if (!this->ReadAnnotation(aNode))
	{
		return 0;
	}

	aNode->InvokeEvent(vtkMRMLScene::NodeAddedEvent, aNode);//vtkMRMLAnnotationNode::DisplayModifiedEvent);

	return 1;
}


int vtkMRMLProbeStorageNode::ReadOneProbe(fstream & fstr, vtkMRMLProbeNode *refNode)
{

  if (refNode == NULL)
    {
    vtkErrorMacro("ReadOneRuler: can't read into a null node");
    return 0;
    }

  // do not read if if we are not in the scene (for example inside snapshot)
  if (!this->GetAddToScene())
    {
    return 1;
    }

  if (!fstr.is_open())
    {
    vtkErrorMacro("ReadOneRuler: file isn't open");
    return 0;
    }

  
  vtkErrorMacro("ReadOneRuler: not implemented yet!");
  return 0;
}
                                                    
//----------------------------------------------------------------------------
int vtkMRMLProbeStorageNode::WriteAnnotationProbeProperties(fstream& of, vtkMRMLProbeNode *refNode)
{
   // put down a header
  if (refNode == NULL)
    {
    vtkWarningMacro("WriteAnnotationProbeProperties: ref node is null");
    return 0;
    }

  of << "# " << this->GetAnnotationStorageType() << "Resolution = " << refNode->GetResolution() << endl;
  of << "# " << this->GetAnnotationStorageType() << "DistanceAnnotationFormat = " << refNode->GetDistanceAnnotationFormat() << endl;
  of << "# " << this->GetAnnotationStorageType() << "Columns = type|line1ID|sel|vis" << endl;

  return 1;

}

//----------------------------------------------------------------------------
int vtkMRMLProbeStorageNode::WriteAnnotationProbeData(fstream& of, vtkMRMLProbeNode *refNode)
{
  if (!refNode)
    {
    vtkWarningMacro("WriteAnnotationProbeData: reference node is null");
    return 0;
    }
  int sel = refNode->GetSelected();
  int vis = refNode->GetVisible(); 
  of << this->GetAnnotationStorageType() << "|" << 0  << "|" << sel << "|" << vis << endl;   

  return 1;
}

//----------------------------------------------------------------------------
/// Write data from the referenced node into the stream
int vtkMRMLProbeStorageNode::WriteAnnotationDataInternal(vtkMRMLNode *refNode, fstream & of)
{

	int retval = this->Superclass::WriteAnnotationDataInternal(refNode,of);
	if (!retval)
	{
		vtkWarningMacro("Ruler WriteAnnotationDataInternal: unable to call superclass WriteData, retval = " << retval);
		return 0;
	}

	// test whether refNode is a valid node to hold a volume
	if ( !( refNode->IsA("vtkMRMLProbeNode") ) )
	{
		vtkErrorMacro("WriteAnnotationDataInternal: Reference node is not a proper vtkMRMLProbeNode");
		return 0;         
	}


	// cast the input nod
	vtkMRMLProbeNode *aNode = dynamic_cast <vtkMRMLProbeNode *> (refNode);

	if (aNode == NULL)
	{
		vtkErrorMacro("WriteAnnotationDataInternal: unable to cast input node " << refNode->GetID() << " to a known annotation line node");
		return 0;
	}

	// Control Points Properties
	if (!WriteAnnotationProbeProperties(of, aNode))
	{
		vtkWarningMacro("Ruler: WriteAnnotationDataInternal: failure in WriteAnnotationProbeProperties");
		return 0;
	}

	if (!WriteAnnotationProbeData(of, aNode))
	{
		vtkWarningMacro("Ruler: WriteAnnotationDataInternal: failure in WriteAnnotationProbeData");
		return 0;
	}

	return 1;
}