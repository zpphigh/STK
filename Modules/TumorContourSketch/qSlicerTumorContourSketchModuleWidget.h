/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerTumorContourSketchModuleWidget_h
#define __qSlicerTumorContourSketchModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerTumorContourSketchModuleExport.h"

class qSlicerTumorContourSketchModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_TUMORCONTOURSKETCH_EXPORT qSlicerTumorContourSketchModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerTumorContourSketchModuleWidget(QWidget *parent=0);
  virtual ~qSlicerTumorContourSketchModuleWidget();

public slots:


protected:
  QScopedPointer<qSlicerTumorContourSketchModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerTumorContourSketchModuleWidget);
  Q_DISABLE_COPY(qSlicerTumorContourSketchModuleWidget);
};

#endif
