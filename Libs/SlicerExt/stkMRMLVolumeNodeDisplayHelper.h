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

class stkMRMLVolumeNodeDisplayHelperPrivate;
class STK_SLICEREXT_EXPORT stkMRMLVolumeNodeDisplayHelper : public QObject
{
	Q_OBJECT

public:
	stkMRMLVolumeNodeDisplayHelper(QObject *parent = 0);
	~stkMRMLVolumeNodeDisplayHelper();	


	static void Display3DVolume(std::string nodeID, vtkColorTransferFunction* colorTransfer = NULL, vtkPiecewiseFunction* piecewiseFunction = NULL);
	static void Display2DSlice(std::string nodeID);
	static void DisplayForeground2DSlice(std::string nodeID);

	static double VolumeToGrayscaleModel(std::string InputVolume, double Threshold, std::string ModelName, double Smooth,	double Decimate, bool SplitNormals,	bool PointNormals, double r, double g, double b);

protected:
	QScopedPointer<stkMRMLVolumeNodeDisplayHelperPrivate> d_ptr;

	

private:	
	Q_DECLARE_PRIVATE(stkMRMLVolumeNodeDisplayHelper);
	Q_DISABLE_COPY(stkMRMLVolumeNodeDisplayHelper);	
};
