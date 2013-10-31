#ifndef __vtkMRMLProbeNode_h
#define __vtkMRMLProbeNode_h

#include "stkSlicerExtExport.h"
#include "vtkMRMLAnnotationLinesNode.h" 

class vtkMatrix4x4;
class vtkAbstractTransform;
class vtkMRMLScene;
class QVector3D;
class QQuaternion;

/// \ingroup Slicer_QtModules_Annotation
class  STK_SLICEREXT_EXPORT vtkMRMLProbeNode : public vtkMRMLAnnotationLinesNode
{
public:
	static vtkMRMLProbeNode *New();
	vtkTypeMacro(vtkMRMLProbeNode,vtkMRMLAnnotationLinesNode);
	// Description:
	// Just prints short summary 
	void PrintAnnotationInfo(ostream& os, vtkIndent indent, int titleFlag = 1);

	//--------------------------------------------------------------------------
	// MRMLNode methods
	//--------------------------------------------------------------------------

	virtual vtkMRMLNode* CreateNodeInstance();
	// Description:
	// Get node XML tag name (like Volume, Model)
	virtual const char* GetNodeTagName() {return "AnnotationProbe";};

	virtual const char* GetIcon() {return ":/Icons/AnnotationDistance.png";};

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



	// Get position of the first point
	void GetHeader(double* position);
	double* GetPosition()
	{
		double position[4] = {0, 0, 0, 1};
		this->GetHeader(position);
		return position;
	}

	// Legacy code
	// Description:
	// get/set the first point position
	double* GetHeader() 
	{
		return this->Position;
	}

	void SetPosition(double* newControl) 
	{ 
		this->Position[0] = newControl[0];
		this->Position[1] = newControl[1];
		this->Position[2] = newControl[2];
		this->Position[3] = newControl[3]; 

		this->Modified();
	}

	void GetTail(double* p);
	double* GetTail()
	{
		double p[3] = {0, 0, 0};
		this->GetTail(p);
		return p;
	}

	int SetControlPoint(int id, double newControl[3]);

	int AddControlPoint(double newControl[3],int selectedFlag, int visibleFlag);

	// Description:
	// get/set the distance annotation format, it's in standard sprintf notation
	vtkGetStringMacro(DistanceAnnotationFormat);
	vtkSetStringMacro(DistanceAnnotationFormat);

	// Description:
	// KP Define - should be part of AnnotationProbeDisplayNode 
	double GetDistanceAnnotationScale();
	void SetDistanceAnnotationScale(double init);

	// Description:
	// get/set the distance annotation visbility
	int GetDistanceAnnotationVisibility();
	void SetDistanceAnnotationVisibility(int flag);

	int SetProbe(vtkIdType line1Id, int sel, int vis);  

	// Description:
	// get/set the resolution (number of subdivisions) of the line.
	vtkGetMacro(Resolution, int);
	vtkSetMacro(Resolution, int);

	// Description:
	// get/set the point representation colour
	double *GetPointColour();
	void SetPointColour( double initColor[3]);

	// Description:
	// get/set the line representation colour
	double *GetLineColour();
	void SetLineColour(double newColor[3]);

	// Description:
	// get/set the distance annotation text colour
	double *GetDistanceAnnotationTextColour();
	void SetDistanceAnnotationTextColour(double initColor[3]);

	// Description:
	// transform utility functions
	virtual void ApplyTransformMatrix(vtkMatrix4x4* transformMatrix);
	virtual void ApplyTransform(vtkAbstractTransform* transform);

	// Description:
	// Create default storage node or NULL if does not have one
	virtual vtkMRMLStorageNode* CreateDefaultStorageNode();  

	//  void Initialize(vtkMRMLScene* mrmlScene);

	int GetVisibility() {return this->GetVisible();}
	void SetVisibility(int flag) {this->SetVisible(flag);}

	double GetDistanceMeasurement();
	void SetDistanceMeasurement(double val);


	/// get/set the id of the model the first and second end of the Probe is constrained upon
	vtkGetStringMacro(ModelID1);
	vtkSetStringMacro(ModelID1);
	vtkGetStringMacro(ModelID2);
	vtkSetStringMacro(ModelID2);

	vtkGetMacro(Power, int);
	vtkSetMacro(Power, int);
	vtkGetMacro(StartTime, int);
	vtkSetMacro(StartTime, int);
	vtkGetMacro(EndTime, int);
	vtkSetMacro(EndTime, int);

	vtkGetMacro(Orientation, QQuaternion*);
	vtkSetMacro(Orientation, QQuaternion*);

	void GetNormal(double* normal)
	{
		normal[0] = this->Normal[0];
		normal[1] = this->Normal[1];
		normal[2] = this->Normal[2];
		/*normal[0] = this->Orientation->vector().x();
		normal[1] = this->Orientation->vector().y();
		normal[2] = this->Orientation->vector().z();*/
	}

	void SetNormal(double p0, double p1, double p2)
	{
		this->Normal[0] = p0;
		this->Normal[1] = p1;
		this->Normal[2] = p2;

		/*QVector3D vec;
		vec.setX(p0);
		vec.setY(p1);
		vec.setZ(p2);*/

		this->Modified();
	}

	vtkGetMacro(Length, double);
	vtkSetMacro(Length, double);

	vtkGetMacro(Visible, int);
	vtkSetMacro(Visible, int);

	vtkGetMacro(FocalLocked, bool);
	vtkSetMacro(FocalLocked, bool);

	vtkGetMacro(VisibleIn2DWidget, bool);
	vtkSetMacro(VisibleIn2DWidget, bool);

	vtkGetMacro(VisibleIn3DWidget, bool);
	vtkSetMacro(VisibleIn3DWidget, bool);

	vtkGetMacro(ColorChanged, bool);

	vtkGetMacro(Current,bool);
	void SetCurrent(bool c)
	{
		this->Current = c;
		this->VisibleIn2DWidget = c;
		this->Modified();
	}

	void GetColor(double* color)
	{
		if (Current)
		{
			color[0] = this->Color[0];
			color[1] = this->Color[1];
			color[2] = this->Color[2];
		}
		else
		{
			color[0] = 1.0;
			color[1] = 1.0;
			color[2] = 1.0;
		}
	}
	void SetColor(int r, int g, int b) 
	{
		if (r > 255 || g > 255 || b > 255 || r < 0 || g < 0 || b < 0)
		{
			vtkErrorMacro("Wrong input argument. Need double numbers in range of [0,1] or integer numbers in range of [0 255]");
			return;
		}
		double R,G,B;
		R = r/255.0;
		G = g/255.0;
		B = b/255.0;

		this->SetColor(R, G, B);
	}
	void SetColor(double r, double g, double b)
	{
		if (r > 255 || g > 255 || b > 255 || r < 0 || g < 0 || b < 0)
		{
			vtkErrorMacro("Wrong input argument. Need double numbers in range of [0,1] or integer numbers in range of [0 255]");
			return;
		}

		this->Color[0] = r;
		this->Color[1] = g;
		this->Color[2] = b;

		this->ColorChanged = true;

		this->Modified();
	}

	enum
	{
		ProbeNodeAddedEvent = 20010,
		ValueModifiedEvent,
	};


protected:
	vtkMRMLProbeNode();
	~vtkMRMLProbeNode();
	vtkMRMLProbeNode(const vtkMRMLProbeNode&);
	void operator=(const vtkMRMLProbeNode&);

	// Description:
	// number of subdivisions on the line
	int Resolution;
	char* DistanceAnnotationFormat;

	

	/// the model ids for the models that the ends of the Probe are constrained
	/// to
	char *ModelID1;
	char *ModelID2;
	double distanceMeasurement;

	int Power;
	int StartTime;
	int EndTime;

private:
	QQuaternion* Orientation;
	double Position[4];
	double Normal[3];
	double Length;
	int Visible;
	bool FocalLocked;

	bool VisibleIn2DWidget;
	bool VisibleIn3DWidget;

	double Color[3];
	bool ColorChanged;

	bool Current;
	
};

#endif