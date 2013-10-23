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

// VolumeDisplayController Logic includes
#include <vtkSlicerVolumeDisplayControllerLogic.h>

// VolumeDisplayController includes
#include "qSlicerVolumeDisplayControllerModule.h"
#include "qSlicerVolumeDisplayControllerModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVolumeDisplayControllerModule, qSlicerVolumeDisplayControllerModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVolumeDisplayControllerModulePrivate
{
public:
  qSlicerVolumeDisplayControllerModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModulePrivate
::qSlicerVolumeDisplayControllerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVolumeDisplayControllerModule methods

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModule
::qSlicerVolumeDisplayControllerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVolumeDisplayControllerModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVolumeDisplayControllerModule::~qSlicerVolumeDisplayControllerModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeDisplayControllerModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeDisplayControllerModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeDisplayControllerModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVolumeDisplayControllerModule::icon()const
{
  return QIcon(":/Icons/VolumeDisplayController.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeDisplayControllerModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVolumeDisplayControllerModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVolumeDisplayControllerModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerVolumeDisplayControllerModule
::createWidgetRepresentation()
{
  return new qSlicerVolumeDisplayControllerModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVolumeDisplayControllerModule::createLogic()
{
  return vtkSlicerVolumeDisplayControllerLogic::New();
}
