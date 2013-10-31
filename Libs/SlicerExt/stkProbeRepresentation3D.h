/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProbeRepresentation3D.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkProbeRepresentation3D - represent the vtkDistanceWidget
// .SECTION Description
// The vtkProbeRepresentation3D is a representation for the
// vtkDistanceWidget. This representation consists of a measuring line (axis)
// and two vtkHandleWidgets to place the end points of the line. Note that
// this particular widget draws its representation in 3D space, so the widget
// can be occluded.

// .SECTION See Also
// vtkDistanceWidget vtkDistanceRepresentation vtkDistanceRepresentation2D


#ifndef __vtkProbeRepresentation3D_h
#define __vtkProbeRepresentation3D_h

#include "stkMeasureRepresentation.h"
#include "stkSlicerExtExport.h"

class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkVectorText;
class vtkFollower;
class vtkBox;
class vtkCylinderSource;
class vtkGlyph3D;
class vtkDoubleArray;
class vtkTransformPolyDataFilter;
class vtkProperty;
class vtkPointHandleRepresentation3D;
class vtkCellPicker;
class vtkLineSource;
class vtkSphereSource;
class vtkConeSource;
class vtkTubeFilter;


class STK_SLICEREXT_EXPORT vtkProbeRepresentation3D : public vtkMeasureRepresentation
{
public:
  // Description:
  // Instantiate class.
  static vtkProbeRepresentation3D *New();

  // Description:
  // Standard VTK methods.
  vtkTypeMacro(vtkProbeRepresentation3D,vtkMeasureRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Satisfy the superclasses API.
  virtual double GetDistance()
    {return this->Distance;}
  virtual void SetDistance(double dis)
    {this->Distance = dis;}

  // Description:
  // Methods to Set/Get the coordinates of the two points defining
  // this representation. Note that methods are available for both
  // display and world coordinates.
  void GetPoint1WorldPosition(double pos[3]);
  double* GetPoint1WorldPosition();
  void GetPoint1DisplayPosition(double pos[3]);
  double* GetPoint1DisplayPosition();
  void SetPoint1WorldPosition(double pos[3]);
  void SetPoint1DisplayPosition(double pos[3]);
  void GetPoint2DisplayPosition(double pos[3]);
  double* GetPoint2DisplayPosition();
  void GetPoint2WorldPosition(double pos[3]);
  double* GetPoint2WorldPosition();
  void SetPoint2WorldPosition(double pos[3]);
  void SetPoint2DisplayPosition(double pos[3]);
  void SetTextColor(double r, double g, double b);

  // Description:
  // This method is used to specify the type of handle representation to
  // use for the three internal vtkHandleWidgets within vtkLineWidget2.
  // To use this method, create a dummy vtkHandleWidget (or subclass),
  // and then invoke this method with this dummy. Then the 
  // vtkLineRepresentation uses this dummy to clone three vtkHandleWidgets
  // of the same type. Make sure you set the handle representation before
  // the widget is enabled. (The method InstantiateHandleRepresentation()
  // is invoked by the vtkLineWidget2.)
  void SetHandleRepresentation(vtkPointHandleRepresentation3D *handle);
  void InstantiateHandleRepresentation();

  // Description:
  // Get the three handle representations used for the vtkLineWidget2. 
  vtkGetObjectMacro(Point1Representation,vtkPointHandleRepresentation3D);
  vtkGetObjectMacro(Point2Representation,vtkPointHandleRepresentation3D);
  vtkGetObjectMacro(LineHandleRepresentation,vtkPointHandleRepresentation3D);

  // Description:
  // Get the end-point (sphere) properties. The properties of the end-points 
  // when selected and unselected can be manipulated.
  vtkGetObjectMacro(EndPointProperty,vtkProperty);
  vtkGetObjectMacro(SelectedEndPointProperty,vtkProperty);

  // Description:
  // Get the end-point (sphere) properties. The properties of the end-points 
  // when selected and unselected can be manipulated.
  vtkGetObjectMacro(EndPoint2Property,vtkProperty);
  vtkGetObjectMacro(SelectedEndPoint2Property,vtkProperty);

  // Description:
  // Get the line properties. The properties of the line when selected
  // and unselected can be manipulated.
  vtkGetObjectMacro(LineProperty,vtkProperty);
  vtkGetObjectMacro(SelectedLineProperty,vtkProperty);

  void SetSelectedProbeColor(double r, double g, double b);
  void SetProbeColor(double r, double g, double b);

  // Description:
  // The tolerance representing the distance to the widget (in pixels) in
  // which the cursor is considered near enough to the line or end point 
  // to be active.
  vtkSetClampMacro(Tolerance,int,1,100);
  vtkGetMacro(Tolerance,int);

  // Description:
  // Set/Get the resolution (number of subdivisions) of the line. A line with
  // resolution greater than one is useful when points along the line are
  // desired; e.g., generating a rake of streamlines.
  void SetResolution(int res);
  int GetResolution();

  // Description:
  // Retrieve the polydata (including points) that defines the line.  The
  // polydata consists of n+1 points, where n is the resolution of the
  // line. These point values are guaranteed to be up-to-date whenever any
  // one of the three handles are moved. To use this method, the user
  // provides the vtkPolyData as an input argument, and the points and
  // polyline are copied into it.
  void GetPolyData(vtkPolyData *pd);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void PlaceWidget(double bounds[6]);
  virtual void BuildRepresentation();
  virtual int ComputeInteractionState(int X, int Y, int modify=0);
  virtual void StartWidgetInteraction(double e[2]);
  virtual void WidgetInteraction(double e[2]);
  virtual double *GetBounds();

  // Description:
  // Methods supporting the rendering process.
  virtual void GetActors(vtkPropCollection *pc);
  virtual void ReleaseGraphicsResources(vtkWindow*);
  virtual int RenderOpaqueGeometry(vtkViewport*);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport*);
  virtual int HasTranslucentPolygonalGeometry();

  //BTX - manage the state of the widget
  enum {Outside=0,
	  OnP1,
	  OnP2,
	  TranslatingP1,
      TranslatingP2,
	  OnLine,
	  Scaling};
  //ETX

  // Description:
  // The interaction state may be set from a widget (e.g., vtkLineWidget2) or
  // other object. This controls how the interaction with the widget
  // proceeds. Normally this method is used as part of a handshaking
  // process with the widget: First ComputeInteractionState() is invoked that
  // returns a state based on geometric considerations (i.e., cursor near a
  // widget feature), then based on events, the widget may modify this
  // further.
  vtkSetClampMacro(InteractionState,int,Outside,Scaling);

  // Description:
  // Sets the visual appearance of the representation based on the
  // state it is in. This state is usually the same as InteractionState.
  virtual void SetRepresentationState(int);
  vtkGetMacro(RepresentationState, int);

  // Description:
  // Overload the superclasses' GetMTime() because internal classes
  // are used to keep the state of the representation.
  virtual unsigned long GetMTime();

  // Description:
  // Overridden to set the rendererer on the internal representations.
  virtual void SetRenderer(vtkRenderer *ren);

  // Description:
  // Show the distance between the points.
  vtkSetMacro( TextAnnotationVisibility, int );
  vtkGetMacro( TextAnnotationVisibility, int );
  vtkBooleanMacro( TextAnnotationVisibility, int );

  // Description:
  // Show the Array between the points.
  vtkSetMacro( ArrowVisibility, int );
  vtkGetMacro( ArrowVisibility, int );
  vtkBooleanMacro( ArrowVisibility, int );

  // Description:
  // Specify the format to use for labelling the line. Note that an empty
  // string results in no label, or a format string without a "%" character
  // will not print the angle value.
  vtkSetStringMacro(TextAnnotationFormat);
  vtkGetStringMacro(TextAnnotationFormat);

  // Description:
  // Scale text (font size along each dimension).
  void SetTextAnnotationScale(double x, double y, double z)
  {
	  double scale[3];
	  scale[0] = x;
	  scale[1] = y;
	  scale[2] = z;
	  this->SetTextAnnotationScale(scale);
  }
  virtual void SetTextAnnotationScale( double scale[3] );
  virtual double * GetTextAnnotationScale();

  // Description:
  // Convenience method to set the line color.
  // Ideally one should use GetLineProperty()->SetColor().
  void SetLineColor(double r, double g, double b);

  // Description:
  // Get the distance annotation property
  virtual vtkProperty *GetTextAnnotationProperty();

  // Description:
  // Get the text actor
  vtkGetObjectMacro(TextActor, vtkFollower);

  void SetTextInputString(std::string text);

  vtkConeSource* GetArrowSource() {return ConeSource;}
  vtkTubeFilter* GetTubeSource() {return TuberSource;}
  vtkConeSource* GetArrow2Source() {return Cone2Source;}
  vtkTubeFilter* GetTube2Source() { return Tuber2Source;}

protected:
  vtkProbeRepresentation3D();
  ~vtkProbeRepresentation3D();

  // The handle and the rep used to close the handles
  vtkPointHandleRepresentation3D *HandleRepresentation;
  vtkPointHandleRepresentation3D *Point1Representation;
  vtkPointHandleRepresentation3D *Point2Representation;
  vtkPointHandleRepresentation3D *LineHandleRepresentation;

  // Manage how the representation appears
  int RepresentationState;

  // the line
  vtkActor          *LineActor;
  vtkPolyDataMapper *LineMapper;
  vtkLineSource     *LineSource;

  // 增加两条line，用来表示两段tube所在的位置
  vtkActor          *Line2Actor;
  vtkPolyDataMapper *Line2Mapper;
  vtkLineSource     *Line2Source;

  vtkActor          *Line3Actor;
  vtkPolyDataMapper *Line3Mapper;
  vtkLineSource     *Line3Source;

  // The + normal cone
  vtkConeSource     *ConeSource;
  vtkPolyDataMapper *ConeMapper;
  vtkActor          *ConeActor;

  // 增加一个cone，用来表示两段不同粗细的tube衔接处的角度
  vtkConeSource     *Cone2Source;
  vtkPolyDataMapper *Cone2Mapper;
  vtkActor          *Cone2Actor;

  // glyphs representing hot spots (e.g., handles)
  vtkActor          **Handle;
  vtkPolyDataMapper **HandleMapper;
  vtkSphereSource   **HandleGeometry;

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *EndPointProperty;
  vtkProperty *SelectedEndPointProperty;
  vtkProperty *EndPoint2Property;
  vtkProperty *SelectedEndPoint2Property;
  vtkProperty *LineProperty;
  vtkProperty *SelectedLineProperty;
  vtkProperty *ConeProperty;
  vtkProperty *SelectedConeProperty;
  vtkProperty *TuberProperty;
  vtkProperty *SelectedTubeProperty;
  void         CreateDefaultProperties();

  // Selection tolerance for the handles and the line
  int Tolerance;

  // Helper members
  int  ClampToBounds;
  void ClampPosition(double x[3]);
  void HighlightPoint(int ptId, int highlight);
  void HighlightLine(int highlight);
  void HighlightArrow(int highlight);
  int  InBounds(double x[3]);
  void SizeHandles();

  // Ivars used during widget interaction to hold initial positions
  double StartP1[3];
  double StartP2[3];
  double StartLineHandle[3];
  double Length;
  double LastEventPosition[3];

  // Support GetBounds() method
  vtkBox *BoundingBox;

  // Need to keep track if we have successfully initialized the display position. 
  // The widget tends to do stuff in world coordinates, put if the renderer has
  // not been assigned, then certain operations do not properly update the display
  // position.
  int InitializedDisplayPosition;

  // Format for the label
  int TextAnnotationVisibility;
  int ArrowVisibility;
  char *TextAnnotationFormat;

  vtkFollower       *TextActor;
  vtkPolyDataMapper *TextMapper;
  vtkVectorText     *TextInput;  
  double             Distance;
  bool               AnnotationTextScaleInitialized;

  vtkTubeFilter     *TuberSource;
  vtkPolyDataMapper *TuberMapper;
  vtkActor          *TuberActor;

  // 在创建一个tube用来表示粗的部分针部;
  vtkTubeFilter     *Tuber2Source;
  vtkPolyDataMapper *Tuber2Mapper;
  vtkActor          *Tuber2Actor;

  //vtkCylinderSource     *CylinderSource;
  //vtkPolyDataMapper		*CylinderMapper;
  //vtkActor				*CylinderActor;
  //vtkProperty			*CylinderProperty;

  vtkCellPicker     *LinePicker;

  char TextString[512];

private:
  vtkProbeRepresentation3D(const vtkProbeRepresentation3D&);  //Not implemented
  void operator=(const vtkProbeRepresentation3D&);  //Not implemented

  double t; // 表示针粗细分别的位置点
};
#endif
