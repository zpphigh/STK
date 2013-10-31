/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See COPYRIGHT.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

 Module:    $RCSfile: vtkMRMLProbeDisplayableManager.h,v $
 Date:      $Date: 2010/07/26 04:48:05 $
 Version:   $Revision: 1.5 $

 =========================================================================auto=*/

#ifndef __vtkMRMLProbeDisplayableManager_h
#define __vtkMRMLProbeDisplayableManager_h

// AnnotationModule includes
#include "stkSlicerExtExport.h"

// AnnotationModule/MRMLDisplayableManager includes
#include "vtkMRMLAnnotationDisplayableManager.h"

class vtkMRMLProbeNode;
class vtkSlicerViewerWidget;
//class vtkMRMLProbeDisplayNode;
class vtkMRMLAnnotationPointDisplayNode;
class vtkMRMLAnnotationLineDisplayNode;
class vtkTextWidget;
class vtkPlane;

/// \ingroup Slicer_QtModules_Annotation
class STK_SLICEREXT_EXPORT vtkMRMLProbeDisplayableManager :
    public vtkMRMLAnnotationDisplayableManager
{
public:

  static vtkMRMLProbeDisplayableManager *New();
  vtkTypeRevisionMacro(vtkMRMLProbeDisplayableManager, vtkMRMLAnnotationDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:

  vtkMRMLProbeDisplayableManager();
  virtual ~vtkMRMLProbeDisplayableManager();

  /// Callback for click in RenderWindow
  virtual void OnClickInRenderWindow(double x, double y, const char *associatedNodeID);
  /// Create a widget.
  virtual vtkAbstractWidget * CreateWidget(vtkMRMLAnnotationNode* node);

  /// Gets called when widget was created
  virtual void OnWidgetCreated(vtkAbstractWidget * widget, vtkMRMLAnnotationNode * node);

  /// Propagate properties of MRML node to widget.
  virtual void PropagateMRMLToWidget(vtkMRMLAnnotationNode* node, vtkAbstractWidget * widget);
  /// Propagate properties of widget to MRML node.
  virtual void PropagateWidgetToMRML(vtkAbstractWidget * widget, vtkMRMLAnnotationNode* node);

  // update the Probe end point positions from the MRML node
  virtual void UpdatePosition(vtkAbstractWidget *widget, vtkMRMLNode *node);

  /// Observe one node
  void SetAndObserveNode(vtkMRMLProbeNode *probeNode);
  /// Observe all associated nodes.
  void SetAndObserveNodes();

private:

  vtkMRMLProbeDisplayableManager(const vtkMRMLProbeDisplayableManager&); /// Not implemented
  void operator=(const vtkMRMLProbeDisplayableManager&); /// Not Implemented

  vtkPlane* plane;

};

#endif

