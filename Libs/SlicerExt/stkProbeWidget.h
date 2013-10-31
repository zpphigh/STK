/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See COPYRIGHT.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer
 Module:    $RCSfile: vtkMRMLAnnotationDisplayableManagerHelper,v $
 Date:      $Date: Aug 4, 2010 10:44:52 AM $
 Version:   $Revision: 1.0 $

 =========================================================================auto=*/

#ifndef vtkProbeWIDGET_H_
#define vtkProbeWIDGET_H_

// ThtkAnnotations includes
#include "stkSlicerExtExport.h"

// VTK includes
#include "stkMeasureWidget.h"

//class vtkAnnotationProbeRepresentation2D;
class vtkProbeRepresentation3D;
class vtkHandleWidget;

/// \ingroup Slicer_QtModules_Annotation
class STK_SLICEREXT_EXPORT vtkProbeWidget :
    public vtkMeasureWidget
{
public:

  static vtkProbeWidget *New();
  vtkTypeRevisionMacro(vtkProbeWidget, vtkMeasureWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Override superclasses' SetEnabled() method because the line
  // widget must enable its internal handle widgets.
  virtual void SetEnabled(int enabling);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkProbeRepresentation3D *r)
  {this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));}

  /*void SetRepresentation(vtkProbeRepresentation2D *r)
  {this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));}*/

  // Description:
  // Return the representation as a vtkLineRepresentation.
  vtkProbeRepresentation3D *GetLineRepresentation()
  {return reinterpret_cast<vtkProbeRepresentation3D*>(this->WidgetRep);}

  void CreateDefaultRepresentation();

  // Description:
  // Methods to change the whether the widget responds to interaction.
  // Overridden to pass the state to component widgets.
  virtual void SetProcessEvents(int);

protected:

  vtkProbeWidget();
  virtual ~vtkProbeWidget();

  //BTX - manage the state of the widget
  int WidgetState;
  enum _WidgetState {Start=0,Active};
  //ETX
  int CurrentHandle;

  // These methods handle events
  static void SelectAction(vtkAbstractWidget*);
  static void TranslateAction(vtkAbstractWidget*);
  static void ScaleAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);
  static void MoveAction(vtkAbstractWidget*);

  // The positioning handle widgets
  vtkHandleWidget *Point1Widget; //first end point
  vtkHandleWidget *Point2Widget; //second end point
  vtkHandleWidget *LineHandle; //used when selecting the line

private:

  vtkProbeWidget(const vtkProbeWidget&); /// Not implemented
  void operator=(const vtkProbeWidget&); /// Not Implemented

};

#endif /* vtkProbeWIDGET_H_ */
