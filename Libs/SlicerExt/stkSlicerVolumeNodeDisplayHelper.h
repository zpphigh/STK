/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// STK includes
#include "stkSlicerExtExport.h"
#include <string>

class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkMRMLVolumeRenderingDisplayNode;
class vtkMRMLVolumeNode;

class STK_SLICEREXT_EXPORT stkSlicerVolumeNodeDisplayHelper
{

public:
	static void Display3DVolume(std::string nodeID, vtkColorTransferFunction* colorTransfer = NULL, vtkPiecewiseFunction* piecewiseFunction = NULL);
	static void Display2DSlice(std::string nodeID);
	static void DisplayForeground2DSlice(std::string nodeID);
	static void Set2DOpacity(double t);
	static double VolumeToGrayscaleModel(std::string InputVolume, double Threshold, std::string ModelName, double Smooth,	double Decimate, bool SplitNormals,	bool PointNormals, double r, double g, double b);
	static vtkMRMLVolumeRenderingDisplayNode* CreateVolumeRenderingDisplayNode(vtkMRMLVolumeNode* volumeNode);
};
