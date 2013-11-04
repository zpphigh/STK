/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAnnotationProbeRepresentation3D.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "stkProbeRepresentation3D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkPolyDataMapper.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkVectorText.h"
#include "vtkFollower.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkCoordinate.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkInteractorObserver.h"
#include "vtkMath.h"
#include "vtkWindow.h"
#include "vtkSmartPointer.h"
#include "vtkBox.h"
#include "vtkGlyph3D.h"
#include "vtkCylinderSource.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkSmartPointer.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkCellPicker.h"
#include "vtkLineSource.h"
#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include "vtkLine.h"
#include "vtkTubeFilter.h"
#include "vtkMRMLModelDisplayableManager.h"

vtkStandardNewMacro(vtkProbeRepresentation3D);

vtkCxxSetObjectMacro(vtkProbeRepresentation3D,HandleRepresentation,vtkPointHandleRepresentation3D);

//----------------------------------------------------------------------
vtkProbeRepresentation3D::vtkProbeRepresentation3D()
{
	this->t = 0.15;  // 在十分之一比例处，针的粗细不同

	// Handle size is in pixels for this widget
	this->HandleSize = 20.0;

	// By default, use one of these handles
	this->HandleRepresentation  = vtkPointHandleRepresentation3D::New();
	this->HandleRepresentation->AllOff();
	this->HandleRepresentation->SetHotSpotSize(0.0);
	this->HandleRepresentation->SetPlaceFactor(1.0);
	this->HandleRepresentation->TranslationModeOn();
	this->Point1Representation = NULL;
	this->Point2Representation = NULL;
	this->LineHandleRepresentation = NULL;
	this->InstantiateHandleRepresentation();

	// Miscellaneous parameters
	this->Tolerance = 5;
	this->Placed = 0;

	// Represent the line
	this->LineSource = vtkLineSource::New();
	this->LineSource->SetResolution(5);
	this->LineMapper = vtkPolyDataMapper::New();
	this->LineMapper->SetInput(this->LineSource->GetOutput());
	this->LineActor = vtkActor::New();
	this->LineActor->SetMapper(this->LineMapper);

	// 在定义一条线，表示针尾较粗部分的直线
	this->Line2Source = vtkLineSource::New();
	this->Line2Source->SetResolution(5);
	this->Line2Mapper = vtkPolyDataMapper::New();
	this->Line2Mapper->SetInput(this->Line2Source->GetOutput());
	this->Line2Actor = vtkActor::New();
	this->Line2Actor->SetMapper(this->Line2Mapper);

	this->Line3Source = vtkLineSource::New();
	this->Line3Source->SetResolution(5);
	this->Line3Mapper = vtkPolyDataMapper::New();
	this->Line3Mapper->SetInput(this->Line3Source->GetOutput());
	this->Line3Actor = vtkActor::New();
	this->Line3Actor->SetMapper(this->Line3Mapper);

	//Create the Tube
	TuberSource = vtkTubeFilter::New();
	TuberSource->SetNumberOfSides(20);
	TuberSource->SetInput(this->Line3Source->GetOutput());
	TuberSource->SetRadius(0.4);
	TuberMapper = vtkPolyDataMapper::New();
	TuberMapper->ScalarVisibilityOff();
	TuberActor = vtkActor::New();
	TuberMapper->SetInput(this->TuberSource->GetOutput());		
	this->TuberActor->SetMapper(this->TuberMapper);

	// 定义另一个tube的输入数据
	Tuber2Source = vtkTubeFilter::New();
	Tuber2Source->SetNumberOfSides(40);
	Tuber2Source->SetInput(this->Line2Source->GetOutput());
	Tuber2Source->SetRadius(0.9);
	Tuber2Mapper = vtkPolyDataMapper::New();
	Tuber2Mapper->ScalarVisibilityOff();
	Tuber2Actor = vtkActor::New();
	Tuber2Mapper->SetInput(this->Tuber2Source->GetOutput());		
	this->Tuber2Actor->SetMapper(this->Tuber2Mapper);

	this->ArrowVisibility = 1;
	this->ConeSource = vtkConeSource::New();
	this->ConeSource->SetResolution(100);
	/*this->ConeSource->SetAngle(12.0);*/
	this->ConeSource->SetRadius(0.4);
	this->ConeSource->SetHeight(1.5);
	this->ConeMapper = vtkPolyDataMapper::New();
	this->ConeMapper->SetInput(this->ConeSource->GetOutput());
	this->ConeActor = vtkActor::New();
	this->ConeActor->SetMapper(this->ConeMapper);

	// 另一个Cone
	this->Cone2Source = vtkConeSource::New();
	this->Cone2Source->SetResolution(100);
	/*his->Cone2Source->SetAngle(20.0);*/
	this->Cone2Source->SetRadius(0.9);
	this->Cone2Source->SetHeight(4);
	this->Cone2Mapper = vtkPolyDataMapper::New();
	this->Cone2Mapper->SetInput(this->Cone2Source->GetOutput());
	this->Cone2Actor = vtkActor::New();
	this->Cone2Actor->SetMapper(this->Cone2Mapper);

	// Create the handles
	this->Handle = new vtkActor* [2];
	this->HandleMapper = new vtkPolyDataMapper* [2];
	this->HandleGeometry = new vtkSphereSource* [2];
	for (int i=0; i<2; i++)
	{
		this->HandleGeometry[i] = vtkSphereSource::New();
		this->HandleGeometry[i]->SetThetaResolution(16);
		this->HandleGeometry[i]->SetPhiResolution(8);
		this->HandleGeometry[i]->SetRadius(0);
		this->HandleMapper[i] = vtkPolyDataMapper::New();
		this->HandleMapper[i]->SetInput(this->HandleGeometry[i]->GetOutput());
		this->Handle[i] = vtkActor::New();
		this->Handle[i]->SetMapper(this->HandleMapper[i]);
	}

	// Set up the initial properties
	this->CreateDefaultProperties();

	// Pass the initial properties to the actors.
	this->Handle[0]->SetProperty(this->EndPointProperty);
	this->Point1Representation->SetProperty(this->EndPointProperty);
	this->Handle[1]->SetProperty(this->EndPoint2Property);
	this->Point2Representation->SetProperty(this->EndPoint2Property);
	this->LineHandleRepresentation->SetProperty(this->EndPointProperty);
	this->LineActor->SetProperty(this->LineProperty);
	this->ConeActor->SetProperty(this->ConeProperty);
	this->Line2Actor->SetProperty(this->LineProperty);
	this->Tuber2Actor->SetProperty(this->TuberProperty);
	this->Cone2Actor->SetProperty(this->ConeProperty);
	this->Line3Actor->SetProperty(this->LineProperty);
	this->TuberActor->SetProperty(this->TuberProperty);

	// Define the point coordinates
	double bounds[6];
	bounds[0] = -0.5;
	bounds[1] = 0.5;
	bounds[2] = -0.5;
	bounds[3] = 0.5;
	bounds[4] = -0.5;
	bounds[5] = 0.5;
	this->PlaceFactor = 1.0; //overload parent's value

	// The distance text annotation
	this->TextAnnotationVisibility = 0;
	this->Distance = 0.0;   
	this->TextAnnotationFormat = new char[8]; 
	sprintf(this->TextAnnotationFormat,"%s","%-#6.3g");  
	this->TextInput = vtkVectorText::New();
	this->TextInput->SetText( "0" );
	this->TextMapper = vtkPolyDataMapper::New();
	this->TextMapper->SetInput( this->TextInput->GetOutput() );
	this->TextActor = vtkFollower::New();
	this->TextActor->SetMapper(this->TextMapper);
	this->TextActor->GetProperty()->SetColor( 1.0, 0.1, 0.0 );  

	// This needs to be initialized before PlaceWidget is called.
	this->InitializedDisplayPosition = 0;

	this->ClampToBounds = 0;

	// The bounding box
	this->BoundingBox = vtkBox::New();

	this->LinePicker = vtkCellPicker::New();
	this->LinePicker->SetTolerance(0.005); //need some fluff
	this->LinePicker->AddPickList( this->LineActor );
	this->LinePicker->PickFromListOn();

	this->RepresentationState = vtkProbeRepresentation3D::Outside;
	this->AnnotationTextScaleInitialized = false;

	// Initial creation of the widget, serves to initialize it.
	// Call PlaceWidget() LAST in the constructor, as this method depends on ivar
	// values.
	this->PlaceWidget(bounds);
}

//----------------------------------------------------------------------
vtkProbeRepresentation3D::~vtkProbeRepresentation3D()
{
	if ( this->HandleRepresentation )
	{
		this->HandleRepresentation->Delete();
	}
	if ( this->Point1Representation )
	{
		this->Point1Representation->Delete();
	}
	if ( this->Point2Representation )
	{
		this->Point2Representation->Delete();
	}
	if ( this->LineHandleRepresentation )
	{
		this->LineHandleRepresentation->Delete();
	}

	this->LineActor->Delete();
	this->LineMapper->Delete();
	this->LineSource->Delete();

	this->Line2Actor->Delete();
	this->Line2Mapper->Delete();
	this->Line2Source->Delete();

	this->Line3Actor->Delete();
	this->Line3Mapper->Delete();
	this->Line3Source->Delete();

	for (int i=0; i<2; i++)
	{
		this->HandleGeometry[i]->Delete();
		this->HandleMapper[i]->Delete();
		this->Handle[i]->Delete();
	}
	delete [] this->Handle;
	delete [] this->HandleMapper;
	delete [] this->HandleGeometry;

	this->EndPointProperty->Delete();
	this->SelectedEndPointProperty->Delete();
	this->EndPoint2Property->Delete();
	this->SelectedEndPoint2Property->Delete();
	this->LineProperty->Delete();
	this->SelectedLineProperty->Delete();
	this->ConeProperty->Delete();
	this->SelectedConeProperty->Delete();

	this->BoundingBox->Delete();

	if (this->TextAnnotationFormat) 
	{
		delete [] this->TextAnnotationFormat;
		this->TextAnnotationFormat = NULL;
	}
	this->TextInput->Delete();
	this->TextMapper->Delete();
	this->TextActor->Delete();
	this->LinePicker->Delete();

	this->TuberSource->Delete();
	this->TuberMapper->Delete();
	this->TuberActor->Delete();
	this->TuberProperty->Delete();
	this->SelectedTubeProperty->Delete();
	this->ConeSource->Delete();
	this->ConeMapper->Delete();
	this->ConeActor->Delete();

	this->Tuber2Source->Delete();
	this->Tuber2Mapper->Delete();
	this->Tuber2Actor->Delete();
	this->Cone2Source->Delete();
	this->Cone2Mapper->Delete();
	this->Cone2Actor->Delete();
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::InstantiateHandleRepresentation()
{
	if ( ! this->Point1Representation )
	{
		this->Point1Representation = this->HandleRepresentation->NewInstance();
		this->Point1Representation->ShallowCopy(this->HandleRepresentation);
	}

	if ( ! this->Point2Representation )
	{
		this->Point2Representation = this->HandleRepresentation->NewInstance();
		this->Point2Representation->ShallowCopy(this->HandleRepresentation);
	}

	if ( ! this->LineHandleRepresentation )
	{
		this->LineHandleRepresentation = this->HandleRepresentation->NewInstance();
		this->LineHandleRepresentation->ShallowCopy(this->HandleRepresentation);
	}
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetResolution(int r)
{ 
	this->LineSource->SetResolution(r); 
}

//----------------------------------------------------------------------
int vtkProbeRepresentation3D::GetResolution()
{ 
	return this->LineSource->GetResolution(); 
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetPolyData(vtkPolyData *pd)
{
	pd->ShallowCopy(this->LineSource->GetOutput());
}

//-- Set/Get position of the three handles -----------------------------
// Point1
//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetPoint1WorldPosition(double pos[3])
{
	this->Point1Representation->GetWorldPosition(pos);
}

double* vtkProbeRepresentation3D::GetPoint1WorldPosition()
{
	return this->Point1Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetPoint1DisplayPosition(double pos[3])
{
	this->Point1Representation->GetDisplayPosition(pos);
}

double* vtkProbeRepresentation3D::GetPoint1DisplayPosition()
{
	return this->Point1Representation->GetDisplayPosition();
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetPoint1WorldPosition(double x[3])
{
	this->Point1Representation->SetWorldPosition(x);
	this->LineSource->SetPoint1(x);
	//double p[3];
	//this->Point1Representation->GetDisplayPosition(p);
	//this->Point1Representation->SetDisplayPosition(p);
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetPoint1DisplayPosition(double x[3])
{
	this->Point1Representation->SetDisplayPosition(x);
	double p[3];
	this->Point1Representation->GetWorldPosition(p);
	this->Point1Representation->SetWorldPosition(p);
	this->LineSource->SetPoint1(p);
}


// Point2
//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetPoint2WorldPosition(double pos[3])
{
	this->Point2Representation->GetWorldPosition(pos);
}

double* vtkProbeRepresentation3D::GetPoint2WorldPosition()
{
	return this->Point2Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetPoint2DisplayPosition(double pos[3])
{
	this->Point2Representation->GetDisplayPosition(pos);
}

double* vtkProbeRepresentation3D::GetPoint2DisplayPosition()
{
	return this->Point2Representation->GetDisplayPosition();
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetPoint2WorldPosition(double x[3])
{
	this->Point2Representation->SetWorldPosition(x);
	this->LineSource->SetPoint2(x);
	//double p[3];
	//this->Point2Representation->GetDisplayPosition(p);
	//this->Point2Representation->SetDisplayPosition(p);
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetPoint2DisplayPosition(double x[3])
{
	this->Point2Representation->SetDisplayPosition(x);
	double p[3];
	this->Point2Representation->GetWorldPosition(p);
	this->Point2Representation->SetWorldPosition(p);
	this->LineSource->SetPoint2(p);
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetRenderer(vtkRenderer* ren)
{
	this->HandleRepresentation->SetRenderer(ren);
	this->Point1Representation->SetRenderer(ren);
	this->Point2Representation->SetRenderer(ren);
	this->LineHandleRepresentation->SetRenderer(ren);
	this->Superclass::SetRenderer(ren);
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::StartWidgetInteraction(double e[2])
{
	// Store the start position
	this->StartEventPosition[0] = e[0];
	this->StartEventPosition[1] = e[1];
	this->StartEventPosition[2] = 0.0;

	// Store the start position
	this->LastEventPosition[0] = e[0];
	this->LastEventPosition[1] = e[1];
	this->LastEventPosition[2] = 0.0;

	// Get the coordinates of the three handles
	this->Point1Representation->GetWorldPosition(this->StartP1);
	this->Point2Representation->GetWorldPosition(this->StartP2);
	this->LineHandleRepresentation->GetWorldPosition(this->StartLineHandle);

	/*if ( this->InteractionState == vtkProbeRepresentation3D::Scaling )
	{
	double dp1[3], dp2[3];
	this->Point1Representation->GetDisplayPosition(dp1);
	this->Point2Representation->GetDisplayPosition(dp2);
	}*/
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::WidgetInteraction(double e[2])
{
	// Process the motion
	if ( this->InteractionState == vtkProbeRepresentation3D::OnLine )
	{
		double x[3], p1[3], p2[3], delta[3], dir[3];

		// Get the new position
		this->LineHandleRepresentation->GetWorldPosition(x);

		// Compute the delta from the previous position
		delta[0] = x[0] - this->StartLineHandle[0];
		delta[1] = x[1] - this->StartLineHandle[1];
		delta[2] = x[2] - this->StartLineHandle[2];

		for (int i=0; i<3; i++)
		{
			p1[i] = this->StartP1[i] + delta[i];
			p2[i] = this->StartP2[i] + delta[i];
		}

		// Compute the orientation of this Probe
		/*double dis;
		dis = sqrt(vtkMath::Distance2BetweenPoints(x, p2));*/

		//double normal[3];
		//normal[0] = (p2[0] - x[0]) / dis;
		//normal[1] = (p2[1] - x[1]) / dis;
		//normal[2] = (p2[2] - x[2]) / dis;

		//for (int i = 0; i < 3; i++)
		//{
		//	p1[i] = p2[i] - normal[i] * this->Distance;  // Compute the second point coordinate according to P2 and the unit orientate vector
		//}

		this->Point1Representation->SetWorldPosition(p1);
		this->Point2Representation->SetWorldPosition(p2);
	}

	//else if ( this->InteractionState == vtkProbeRepresentation3D::Scaling )
	//{//scale about the center of the widget
	//	double p1[3], p2[3], center[3];

	//	this->Point1Representation->GetWorldPosition(p1);
	//	this->Point2Representation->GetWorldPosition(p2);

	//	double delta = sqrt((this->StartEventPosition[0]-e[0])*(this->StartEventPosition[0]-e[0])+
	//		(this->StartEventPosition[1]-e[1])*(this->StartEventPosition[1]-e[1]));

	//	double sf=1.0;
	//	if ( this->Length != 0.0 )
	//	{
	//		sf = 1.0 + delta/this->Length;
	//	}
	//	if ( (e[1]-this->LastEventPosition[1]) < 0.0 )
	//	{
	//		sf = 1/sf;
	//	}

	//	for (int i=0; i<3; i++)
	//	{
	//		center[i] = (p1[i]+p2[i]) / 2.0;
	//		p1[i] = center[i] + (p1[i]-center[i])*sf;
	//		p2[i] = center[i] + (p2[i]-center[i])*sf;
	//	}

	//	this->Point1Representation->SetWorldPosition(p1);
	//	this->Point2Representation->SetWorldPosition(p2);
	//}

	//else if ( this->InteractionState == vtkProbeRepresentation3D::TranslatingP1 )
	//{
	//	double p1[3], p2[3], normal[3], newP1[3];
	//	// Get the new position
	//	this->Point1Representation->GetWorldPosition(p1);
	//	this->Point2Representation->GetWorldPosition(p2);

	//	double dis = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
	//	normal[0] = (p2[0]-p1[0])/dis;
	//	normal[1] = (p2[1]-p1[1])/dis;
	//	normal[2] = (p2[2]-p1[2])/dis;

	//	newP1[0] = p2[0] - this->Distance*normal[0];
	//	newP1[1] = p2[1] - this->Distance*normal[1];
	//	newP1[2] = p2[2] - this->Distance*normal[2];

	//	this->Point1Representation->SetWorldPosition(newP1);
	//}

	//else if ( this->InteractionState == vtkProbeRepresentation3D::TranslatingP2 )
	//{
	//	double p1[3], p2[3], normal[3], newP2[3];
	//	// Get the new position
	//	this->Point1Representation->GetWorldPosition(p1);
	//	this->Point2Representation->GetWorldPosition(p2);

	//	double dis = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
	//	normal[0] = (p2[0]-p1[0])/dis;
	//	normal[1] = (p2[1]-p1[1])/dis;
	//	normal[2] = (p2[2]-p1[2])/dis;

	//	newP2[0] = p1[0] + this->Distance*normal[0];
	//	newP2[1] = p1[1] + this->Distance*normal[1];
	//	newP2[2] = p1[2] + this->Distance*normal[2];

	//	this->Point2Representation->SetWorldPosition(newP2);
	//}

	// Store the start position
	this->LastEventPosition[0] = e[0];
	this->LastEventPosition[1] = e[1];
	this->LastEventPosition[2] = 0.0;
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::PlaceWidget(double bds[6])
{
	int i;
	double bounds[6], center[3];

	double placeFactor = this->PlaceFactor;
	this->PlaceFactor = 1.0;
	this->AdjustBounds(bds, bounds, center);
	this->PlaceFactor = placeFactor;

	for (i=0; i<6; i++)
	{
		this->InitialBounds[i] = bounds[i];
	}
	this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
		(bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
		(bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

	// When PlaceWidget() is invoked, the widget orientation is preserved, but it
	// is allowed to translate and scale. This means it is centered in the
	// bounding box, and the representation scales itself to intersect the sides
	// of the bounding box. Thus we have to determine where Point1 and Point2
	// intersect the bounding box.
	double p1[3], p2[3], r[3], o[3], t, placedP1[3], placedP2[3];
	this->LineSource->GetPoint1(p1);
	this->LineSource->GetPoint2(p2);

	// Okay, this looks really weird, we are shooting rays from OUTSIDE
	// the bounding box back towards it. This is because the IntersectBox()
	// method computes intersections only if the ray originates outside the
	// bounding box.

	r[0] = this->InitialLength * (p1[0] - p2[0]);
	r[1] = this->InitialLength * (p1[1] - p2[1]);
	r[2] = this->InitialLength * (p1[2] - p2[2]);
	o[0] = center[0] - r[0];
	o[1] = center[1] - r[1];
	o[2] = center[2] - r[2];
	vtkBox::IntersectBox(bounds,o,r,placedP1,t);
	this->SetPoint1WorldPosition(placedP1);

	r[0] = this->InitialLength * (p2[0] - p1[0]);
	r[1] = this->InitialLength * (p2[1] - p1[1]);
	r[2] = this->InitialLength * (p2[2] - p1[2]);
	o[0] = center[0] - r[0];
	o[1] = center[1] - r[1];
	o[2] = center[2] - r[2];
	vtkBox::IntersectBox(bounds,o,r,placedP2,t);
	this->SetPoint2WorldPosition(placedP2);

	// Initialize the center point
	this->LineHandleRepresentation->SetWorldPosition(center);

	/*double dis;
	dis = sqrt(vtkMath::Distance2BetweenPoints(placedP1, placedP2));
	this->Distance = dis;*/

	/*double dir[3];
	dir[0] = placedP2[0] - center[0];
	dir[1] = placedP2[1] - center[1];
	dir[2] = placedP2[2] - center[2];

	double len;
	len = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

	double unit[3];
	unit[0] = dir[0] / len;
	unit[1] = dir[1] / len;
	unit[2] = dir[2] / len;

	placedP1[0] = placedP2[0] - unit[0] * this->Distance;
	placedP1[1] = placedP2[1] - unit[1] * this->Distance;
	placedP1[2] = placedP2[2] - unit[2] * this->Distance;

	this->SetPoint1WorldPosition(placedP1);*/

	// Position the handles at the end of the lines
	this->Placed = 1;
	this->ValidPick = 1;
	this->BuildRepresentation();
}


//----------------------------------------------------------------------------
int vtkProbeRepresentation3D::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
	// Check if we are on end points. Use the handles to determine this.
	int p1State = this->Point1Representation->ComputeInteractionState(X,Y,0);
	int p2State = this->Point2Representation->ComputeInteractionState(X,Y,0);
	if ( p1State == vtkHandleRepresentation::Nearby )
	{
		this->InteractionState = vtkProbeRepresentation3D::OnP1;
		this->SetRepresentationState(vtkProbeRepresentation3D::OnP1);
	}
	else if ( p2State == vtkHandleRepresentation::Nearby )
	{
		this->InteractionState = vtkProbeRepresentation3D::OnP2;
		this->SetRepresentationState(vtkProbeRepresentation3D::OnP2);
	}
	else
	{
		this->InteractionState = vtkProbeRepresentation3D::Outside;
	}

	// Okay if we're near a handle return, otherwise test the line
	if ( this->InteractionState != vtkProbeRepresentation3D::Outside )
	{
		return this->InteractionState;
	}

	// Check if we are on edges
	double pos1[3], pos2[3];
	this->GetPoint1DisplayPosition(pos1);
	this->GetPoint2DisplayPosition(pos2);

	double p1[3], p2[3], xyz[3];
	double t, closest[3];
	xyz[0] = static_cast<double>(X);
	xyz[1] = static_cast<double>(Y);
	p1[0] = static_cast<double>(pos1[0]);
	p1[1] = static_cast<double>(pos1[1]);
	p2[0] = static_cast<double>(pos2[0]);
	p2[1] = static_cast<double>(pos2[1]);
	xyz[2] = p1[2] = p2[2] = 0.0;

	double tol2 = this->Tolerance*this->Tolerance;

	int onLine = (vtkLine::DistanceToLine(xyz,p1,p2,t,closest) <= tol2);
	if ( onLine && t < 1.0 && t > 0.0 )
	{
		this->InteractionState = vtkProbeRepresentation3D::OnLine;
		this->SetRepresentationState(vtkProbeRepresentation3D::OnLine);
		this->GetPoint1WorldPosition(pos1);
		this->GetPoint2WorldPosition(pos2);

		this->LinePicker->Pick(X,Y,0.0,this->Renderer);
		this->LinePicker->GetPickPosition(closest);
		this->LineHandleRepresentation->SetWorldPosition(closest);
	}
	else
	{
		this->InteractionState = vtkProbeRepresentation3D::Outside;
		this->SetRepresentationState(vtkProbeRepresentation3D::Outside);
	}

	return this->InteractionState;
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SetRepresentationState(int state)
{
	if (this->RepresentationState == state)
	{
		return;
	}

	state = (state < vtkProbeRepresentation3D::Outside ?
		vtkProbeRepresentation3D::Outside : 
	(state > vtkProbeRepresentation3D::Scaling ?
		vtkProbeRepresentation3D::Scaling : state));

	this->RepresentationState = state;
	this->Modified();

	if ( state == vtkProbeRepresentation3D::Outside )
	{
		this->HighlightPoint(0,0);
		this->HighlightPoint(1,0);
		this->HighlightLine(0);
		this->HighlightArrow(0);
	}
	else if ( state == vtkProbeRepresentation3D::OnP1 )
	{
		this->HighlightPoint(0,1);
		this->HighlightPoint(1,0);
		this->HighlightLine(0);
		this->HighlightArrow(0);
	}
	else if ( state == vtkProbeRepresentation3D::OnP2 )
	{
		this->HighlightPoint(0,0);
		this->HighlightPoint(1,0);
		this->HighlightLine(0);
		this->HighlightArrow(1);
	}
	else if ( state == vtkProbeRepresentation3D::OnLine )
	{
		this->HighlightPoint(0,1);
		this->HighlightPoint(1,1);
		this->HighlightLine(1);
		this->HighlightArrow(1);
	}
	else 
	{
		this->HighlightPoint(0,1);
		this->HighlightPoint(1,1);
		this->HighlightLine(1);
		this->HighlightArrow(1);
	}
}

//----------------------------------------------------------------------
double *vtkProbeRepresentation3D::GetBounds()
{
	this->BuildRepresentation();
	this->BoundingBox->SetBounds(this->LineActor->GetBounds());
	this->BoundingBox->SetBounds(this->TuberActor->GetBounds());
	this->BoundingBox->AddBounds(this->Handle[0]->GetBounds());
	this->BoundingBox->AddBounds(this->Handle[1]->GetBounds());

	return this->BoundingBox->GetBounds();
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::CreateDefaultProperties()
{
	// Endpoint properties
	this->EndPointProperty = vtkProperty::New();
	this->EndPointProperty->SetColor(193/255.0, 193/255.0, 1.0);

	this->SelectedEndPointProperty = vtkProperty::New();
	this->SelectedEndPointProperty->SetColor(1,1,0);

	this->EndPoint2Property = vtkProperty::New();
	this->EndPoint2Property->SetColor(193/255.0, 193/255.0, 1.0);

	this->SelectedEndPoint2Property = vtkProperty::New();
	this->SelectedEndPoint2Property->SetColor(1,1,0);

	// Line properties
	this->LineProperty = vtkProperty::New();
	this->LineProperty->SetAmbient(1.0);
	this->LineProperty->SetAmbientColor(193/255.0, 193/255.0, 1.0);
	this->LineProperty->SetLineWidth(2.0);

	this->SelectedLineProperty = vtkProperty::New();
	this->SelectedLineProperty->SetAmbient(1.0);
	this->SelectedLineProperty->SetAmbientColor(1.0,1.0,0.0);
	this->SelectedLineProperty->SetLineWidth(2.0);

	this->ConeProperty = vtkProperty::New();
	this->ConeProperty->SetColor(193/255.0, 193/255.0, 1.0);

	this->SelectedConeProperty = vtkProperty::New();
	this->SelectedConeProperty->SetColor(1.0,1.0,0.0);

	this->TuberProperty = vtkProperty::New();
	this->TuberProperty->SetColor(193/255.0, 193/255.0, 1.0);

	this->SelectedTubeProperty = vtkProperty::New();
	this->SelectedTubeProperty->SetColor(1.0,1.0,0.0);
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SizeHandles()
{
	// The SizeHandles() method depends on the LastPickPosition data member.
	double radius = this->vtkWidgetRepresentation::SizeHandlesInPixels(0.05,this->LineSource->GetPoint1());
	this->HandleGeometry[0]->SetRadius(radius);

	radius = this->vtkWidgetRepresentation::SizeHandlesInPixels(0.05,this->LineSource->GetPoint2());
	this->HandleGeometry[1]->SetRadius(radius);
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::BuildRepresentation()
{
	// Rebuild only if necessary
	if ( this->GetMTime() > this->BuildTime ||
		this->Point1Representation->GetMTime() > this->BuildTime ||
		this->Point2Representation->GetMTime() > this->BuildTime ||
		this->LineHandleRepresentation->GetMTime() > this->BuildTime ||
		(this->Renderer && this->Renderer->GetVTKWindow()/* &&
		(this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime ||
		this->Renderer->GetActiveCamera()->GetMTime() > this->BuildTime)*/) )
	{
		if ( ! this->InitializedDisplayPosition && this->Renderer )
		{
			this->SetPoint1WorldPosition(this->LineSource->GetPoint1());
			this->SetPoint2WorldPosition(this->LineSource->GetPoint2());

			this->ValidPick = 1;
			this->InitializedDisplayPosition = 1;
		}

		// Make sure that tolerance is consistent between handles and this representation
		this->Point1Representation->SetTolerance(this->Tolerance);
		this->Point2Representation->SetTolerance(this->Tolerance);

		this->LineHandleRepresentation->SetTolerance(this->Tolerance);

		// Retrieve end point information
		double x1[3], x2[3];
		this->GetPoint1WorldPosition(x1);
		this->LineSource->SetPoint1(x1);
		this->HandleGeometry[0]->SetCenter(x1);

		this->GetPoint2WorldPosition(x2);
		this->LineSource->SetPoint2(x2);
		this->HandleGeometry[1]->SetCenter(x2);

		double normal[3];
		double N = sqrt(vtkMath::Distance2BetweenPoints(x1,x2));
		normal[0] = (x1[0] - x2[0])/N;
		normal[1] = (x1[1] - x2[1])/N;
		normal[2] = (x1[2] - x2[2])/N;
		
		double x3[3];
		x3[0] = x1[0] - this->ConeSource->GetHeight() * normal[0];
		x3[1] = x1[1] - this->ConeSource->GetHeight() * normal[1];
		x3[2] = x1[2] - this->ConeSource->GetHeight() * normal[2];
		this->Line3Source->SetPoint1(x3);
		this->Line3Source->SetPoint2(x2);

		// 圆锥的center位于顶点和底面连线的中心点
		double cone1center[3];
		cone1center[0] = x1[0] - 0.5 * this->ConeSource->GetHeight() * normal[0];
		cone1center[1] = x1[1] - 0.5 * this->ConeSource->GetHeight() * normal[1];
		cone1center[2] = x1[2] - 0.5 * this->ConeSource->GetHeight() * normal[2];

		this->ConeSource->SetDirection(normal);
		this->ConeSource->SetCenter(cone1center); // 针尖点定义为rep的第1个点位置

		double x4[3];
		x4[0] = x1[0] * (1 - t) + t * x2[0];   // t = 0.1, 将第二个圆锥的底面位置设在总长度的0.1位置处
		x4[1] = x1[1] * (1 - t) + t * x2[1];
		x4[2] = x1[2] * (1 - t) + t * x2[2];

		this->Line2Source->SetPoint1(x4);
		this->Line2Source->SetPoint2(x2);

		// 第二个圆锥的中心点
		double cone2center[3];
		double len2 = sqrt(vtkMath::Distance2BetweenPoints(x1, x4));
		double t1 = 0.5 * this->Cone2Source->GetHeight()/len2;
		cone2center[0] = (1 - t1) * x4[0] + t1 * x1[0];
		cone2center[1] = (1 - t1) * x4[1] + t1 * x1[1];
		cone2center[2] = (1 - t1) * x4[2] + t1 * x1[2];

		this->Cone2Source->SetDirection(normal);
		this->Cone2Source->SetCenter(cone2center); // 第二个圆锥位置

		this->TextInput->SetText( TextString );
		this->TextActor->SetPosition( x1 );

		if (this->Renderer)
		{
			this->TextActor->SetCamera( this->Renderer->GetActiveCamera() );
		}

		if (!this->AnnotationTextScaleInitialized)
		{
			//// If a font size hasn't been specified by the user, scale the text 
			//// (font size) according to the length of the line widget.
			//this->TextActor->SetScale( 
			//	this->Distance/10.0, this->Distance/10.0, this->Distance/10.0 );

			this->TextActor->SetScale(5.0, 5.0, 5.0 );
		}

		this->SizeHandles();
		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::HighlightPoint(int ptId, int highlight)
{
	if ( ptId == 0 )
	{
		if ( highlight )
		{
			this->Handle[0]->SetProperty(this->SelectedEndPointProperty);
			this->Point1Representation->SetSelectedProperty(this->SelectedEndPointProperty);
		}
		else
		{
			this->Handle[0]->SetProperty(this->EndPointProperty);
			this->Point1Representation->SetProperty(this->EndPointProperty);
		}
	}
	else if ( ptId == 1 )
	{
		if ( highlight )
		{
			this->Handle[1]->SetProperty(this->SelectedEndPoint2Property);
			this->Point2Representation->SetSelectedProperty(this->SelectedEndPoint2Property);
		}
		else
		{
			this->Handle[1]->SetProperty(this->EndPoint2Property);
			this->Point2Representation->SetProperty(this->EndPoint2Property);
		}
	}
	else //if ( ptId == 2 )
	{
		if ( highlight )
		{
			this->LineHandleRepresentation->SetSelectedProperty(this->SelectedEndPointProperty);
		}
		else
		{
			this->LineHandleRepresentation->SetProperty(this->EndPointProperty);
		}
	}
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::HighlightLine(int highlight)
{
	if ( highlight )
	{
		this->LineActor->SetProperty(this->SelectedLineProperty);
		this->TuberActor->SetProperty(this->SelectedTubeProperty);
		this->Tuber2Actor->SetProperty(this->SelectedTubeProperty);
	}
	else
	{
		this->LineActor->SetProperty(this->LineProperty);
		this->TuberActor->SetProperty(this->TuberProperty);
		this->Tuber2Actor->SetProperty(this->TuberProperty);
	}
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::HighlightArrow(int highlight)
{
	if ( highlight )
	{
		this->ConeActor->SetProperty(this->SelectedConeProperty);
		this->Cone2Actor->SetProperty(this->SelectedConeProperty);
	}
	else
	{
		this->ConeActor->SetProperty(this->ConeProperty);
		this->Cone2Actor->SetProperty(this->ConeProperty);
	}
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SetLineColor(double r, double g, double b)
{
	if(this->GetLineProperty())
	{
		this->GetLineProperty()->SetColor(r, g, b);
	}
}  

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::ClampPosition(double x[3])
{
	for (int i=0; i<3; i++)
	{
		if ( x[i] < this->InitialBounds[2*i] )
		{
			x[i] = this->InitialBounds[2*i];
		}
		if ( x[i] > this->InitialBounds[2*i+1] )
		{
			x[i] = this->InitialBounds[2*i+1];
		}
	}
}

//----------------------------------------------------------------------------
int vtkProbeRepresentation3D::InBounds(double x[3])
{
	for (int i=0; i<3; i++)
	{
		if ( x[i] < this->InitialBounds[2*i] ||
			x[i] > this->InitialBounds[2*i+1] )
		{
			return 0;
		}
	}
	return 1;
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::GetActors(vtkPropCollection *pc)
{
	this->LineActor->GetActors(pc);
	this->TuberActor->GetActors(pc);
	this->Handle[0]->GetActors(pc);
	this->Handle[1]->GetActors(pc);
	this->TextActor->GetActors(pc);
	this->ConeActor->GetActors(pc);
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::ReleaseGraphicsResources(vtkWindow *w)
{
	this->LineActor->ReleaseGraphicsResources(w);
	this->Line2Actor->ReleaseGraphicsResources(w);
	this->TuberActor->ReleaseGraphicsResources(w);
	this->Tuber2Actor->ReleaseGraphicsResources(w);
	this->Handle[0]->ReleaseGraphicsResources(w);
	this->Handle[1]->ReleaseGraphicsResources(w);
	this->TextActor->ReleaseGraphicsResources(w);
	this->ConeActor->ReleaseGraphicsResources(w);
	this->Cone2Actor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------------
int vtkProbeRepresentation3D::RenderOpaqueGeometry(vtkViewport *v)
{
	int count=0;
	this->BuildRepresentation();
	count += this->LineActor->RenderOpaqueGeometry(v);
	count += this->Line2Actor->RenderOpaqueGeometry(v);
	count += this->TuberActor->RenderOpaqueGeometry(v);
	count += this->Tuber2Actor->RenderOpaqueGeometry(v);
	count += this->Handle[0]->RenderOpaqueGeometry(v);
	//count += this->Handle[1]->RenderOpaqueGeometry(v);
	if (this->TextAnnotationVisibility)
	{
		count += this->TextActor->RenderOpaqueGeometry(v);
	}
	if (this->ArrowVisibility)
	{
		count += this->ConeActor->RenderOpaqueGeometry(v);
		count += this->Cone2Actor->RenderOpaqueGeometry(v);
	}

	return count;
}

//----------------------------------------------------------------------------
int vtkProbeRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport *v)
{
	int count=0;
	this->BuildRepresentation();
	count += this->LineActor->RenderTranslucentPolygonalGeometry(v);
	count += this->Line2Actor->RenderTranslucentPolygonalGeometry(v);
	count += this->TuberActor->RenderTranslucentPolygonalGeometry(v);
	count += this->Tuber2Actor->RenderTranslucentPolygonalGeometry(v);
	count += this->Handle[0]->RenderTranslucentPolygonalGeometry(v);
	count += this->Handle[1]->RenderTranslucentPolygonalGeometry(v);
	count += this->ConeActor->RenderTranslucentPolygonalGeometry(v);
	count += this->Cone2Actor->RenderTranslucentPolygonalGeometry(v);
	if (this->TextAnnotationVisibility)
	{
		count += this->TextActor->RenderTranslucentPolygonalGeometry(v);
	}

	return count;
}

//----------------------------------------------------------------------------
int vtkProbeRepresentation3D::HasTranslucentPolygonalGeometry()
{
	int result=0;
	this->BuildRepresentation();
	result |= this->LineActor->HasTranslucentPolygonalGeometry();
	result |= this->Line2Actor->HasTranslucentPolygonalGeometry();
	result |= this->TuberActor->HasTranslucentPolygonalGeometry();
	result |= this->Tuber2Actor->HasTranslucentPolygonalGeometry();
	result |= this->Handle[0]->HasTranslucentPolygonalGeometry();
	result |= this->Handle[1]->HasTranslucentPolygonalGeometry();
	result |= this->ConeActor->HasTranslucentPolygonalGeometry();
	result |= this->Cone2Actor->HasTranslucentPolygonalGeometry();
	if (this->TextAnnotationVisibility)
	{
		result |= this->TextActor->HasTranslucentPolygonalGeometry();
	}

	return result;
}

//----------------------------------------------------------------------------
unsigned long vtkProbeRepresentation3D::GetMTime()
{
	unsigned long mTime=this->Superclass::GetMTime();
	unsigned long mTime2=this->Point1Representation->GetMTime();
	mTime = ( mTime2 > mTime ? mTime2 : mTime );
	mTime2=this->Point2Representation->GetMTime();
	mTime = ( mTime2 > mTime ? mTime2 : mTime );
	mTime2=this->LineHandleRepresentation->GetMTime();
	mTime = ( mTime2 > mTime ? mTime2 : mTime );

	return mTime;
}

//----------------------------------------------------------------------
void vtkProbeRepresentation3D::SetTextAnnotationScale( double scale[3] )
{
	this->TextActor->SetScale( scale );
	this->AnnotationTextScaleInitialized = true;
}

//----------------------------------------------------------------------
double * vtkProbeRepresentation3D::GetTextAnnotationScale()
{
	return this->TextActor->GetScale();
}

//----------------------------------------------------------------------------
vtkProperty * vtkProbeRepresentation3D::GetTextAnnotationProperty()
{
	return this->TextActor->GetProperty();
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	if ( this->LineProperty )
	{
		os << indent << "Line Property: " << this->LineProperty << "\n";
	}
	else
	{
		os << indent << "Line Property: (none)\n";
	}
	if ( this->SelectedLineProperty )
	{
		os << indent << "Selected Line Property: "
			<< this->SelectedLineProperty << "\n";
	}
	else
	{
		os << indent << "Selected Line Property: (none)\n";
	}

	if ( this->EndPointProperty )
	{
		os << indent << "End Point Property: " << this->EndPointProperty << "\n";
	}
	else
	{
		os << indent << "End Point Property: (none)\n";
	}
	if ( this->SelectedEndPointProperty )
	{
		os << indent << "Selected End Point Property: " << this->SelectedEndPointProperty << "\n";
	}
	else
	{
		os << indent << "Selected End Point Property: (none)\n";
	}

	if ( this->EndPoint2Property )
	{
		os << indent << "End Point Property: " << this->EndPoint2Property << "\n";
	}
	else
	{
		os << indent << "End Point Property: (none)\n";
	}
	if ( this->SelectedEndPoint2Property )
	{
		os << indent << "Selected End Point Property: " << this->SelectedEndPoint2Property << "\n";
	}
	else
	{
		os << indent << "Selected End Point Property: (none)\n";
	}  

	os << indent << "Tolerance: " << this->Tolerance << "\n";

	os << indent << "Constrain To Bounds: "
		<< (this->ClampToBounds ? "On\n" : "Off\n");

	int res = this->LineSource->GetResolution();
	double *pt1 = this->LineSource->GetPoint1();
	double *pt2 = this->LineSource->GetPoint2();

	os << indent << "Resolution: " << res << "\n";
	os << indent << "Point 1: (" << pt1[0] << ", "
		<< pt1[1] << ", "
		<< pt1[2] << ")\n";
	os << indent << "Point 2: (" << pt2[0] << ", "
		<< pt2[1] << ", "
		<< pt2[2] << ")\n";

	os << indent << "Point1 Representation: ";
	this->Point1Representation->PrintSelf(os,indent.GetNextIndent());

	os << indent << "Point2 Representation: ";
	this->Point2Representation->PrintSelf(os,indent.GetNextIndent());

	os << indent << "Line Handle Representation: ";
	this->LineHandleRepresentation->PrintSelf(os,indent.GetNextIndent());

	os << indent << "Representation State: " << this->RepresentationState << "\n";

	os << indent << "TextAnnotationVisibility: ";
	if ( this->TextAnnotationVisibility )
	{
		os << this->TextAnnotationVisibility << "\n";
	}
	else
	{
		os << "(none)\n";
	}

	os << indent << "TextAnnotationFormat: ";
	if ( this->TextAnnotationFormat )
	{
		os << this->TextAnnotationFormat << "\n";
	}
	else
	{
		os << "(none)\n";
	}  

	os << indent << "TextActor: ";
	if ( this->TextActor )
	{
		os << this->TextActor << "\n";
	}
	else
	{
		os << "(none)\n";
	}

	// this->InteractionState is printed in superclass
	// this is commented to avoid PrintSelf errors
}

void vtkProbeRepresentation3D::SetTextColor(double r, double g, double b)
{
	this->TextActor->GetProperty()->SetColor(r,g,b);
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SetTextInputString(std::string text)
{	
	int len = text.length();
	char input[512] = {NULL};
	for (int i = 0; i < len; i++)
	{
		input[i] = text[i];
	}
	sprintf_s(TextString, this->TextAnnotationFormat, input);
	BuildRepresentation();
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SetSelectedProbeColor(double r, double g, double b)
{
	this->SelectedTubeProperty->SetColor(r, g, b);
	this->SelectedConeProperty->SetColor(r, g, b);
	/*this->BuildRepresentation();*/
}

//----------------------------------------------------------------------------
void vtkProbeRepresentation3D::SetProbeColor(double r, double g, double b)
{
	this->TuberProperty->SetColor(r, g, b);
    this->ConeProperty->SetColor(r, g, b);
	/*this->BuildRepresentation();*/
}