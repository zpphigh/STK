/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/

#pragma once

// Qt includes
#include <QObject>


// STK includes
#include "stkSlicerExtExport.h"

class vtkPiecewiseFunction;
class vtkColorTransferFunction;

class stkSlicerVolumeNodeDisplayHelperPrivate;
class STK_SLICEREXT_EXPORT stkSlicerVolumeNodeDisplayHelper : public QObject
{
	Q_OBJECT

public:
	stkSlicerVolumeNodeDisplayHelper(QObject *parent = 0);
	~stkSlicerVolumeNodeDisplayHelper();	


	static void Display3DVolume(std::string nodeID, vtkColorTransferFunction* colorTransfer = NULL, vtkPiecewiseFunction* piecewiseFunction = NULL);
	static void Display2DSlice(std::string nodeID);
	static void DisplayForeground2DSlice(std::string nodeID);

	static double VolumeToGrayscaleModel(std::string InputVolume, double Threshold, std::string ModelName, double Smooth,	double Decimate, bool SplitNormals,	bool PointNormals, double r, double g, double b);

protected:
	QScopedPointer<stkSlicerVolumeNodeDisplayHelperPrivate> d_ptr;

	

private:	
	Q_DECLARE_PRIVATE(stkSlicerVolumeNodeDisplayHelper);
	Q_DISABLE_COPY(stkSlicerVolumeNodeDisplayHelper);	
};
