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

// Qt includes
#include <QtPlugin>

// TumorContourSketch Logic includes
#include <vtkSlicerTumorContourSketchLogic.h>

// TumorContourSketch includes
#include "qSlicerTumorContourSketchModule.h"
#include "qSlicerTumorContourSketchModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerTumorContourSketchModule, qSlicerTumorContourSketchModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerTumorContourSketchModulePrivate
{
public:
  qSlicerTumorContourSketchModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerTumorContourSketchModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModulePrivate
::qSlicerTumorContourSketchModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerTumorContourSketchModule methods

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModule
::qSlicerTumorContourSketchModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerTumorContourSketchModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModule::~qSlicerTumorContourSketchModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerTumorContourSketchModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerTumorContourSketchModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerTumorContourSketchModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerTumorContourSketchModule::icon()const
{
  return QIcon(":/Icons/TumorContourSketch.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerTumorContourSketchModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerTumorContourSketchModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerTumorContourSketchModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerTumorContourSketchModule
::createWidgetRepresentation()
{
  return new qSlicerTumorContourSketchModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerTumorContourSketchModule::createLogic()
{
  return vtkSlicerTumorContourSketchLogic::New();
}
