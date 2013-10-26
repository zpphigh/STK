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

// FiducialMarkerRegistration Logic includes
#include <vtkSlicerFiducialMarkerRegistrationLogic.h>

// FiducialMarkerRegistration includes
#include "qSlicerFiducialMarkerRegistrationModule.h"
#include "qSlicerFiducialMarkerRegistrationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerFiducialMarkerRegistrationModule, qSlicerFiducialMarkerRegistrationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFiducialMarkerRegistrationModulePrivate
{
public:
  qSlicerFiducialMarkerRegistrationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFiducialMarkerRegistrationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFiducialMarkerRegistrationModulePrivate
::qSlicerFiducialMarkerRegistrationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFiducialMarkerRegistrationModule methods

//-----------------------------------------------------------------------------
qSlicerFiducialMarkerRegistrationModule
::qSlicerFiducialMarkerRegistrationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFiducialMarkerRegistrationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFiducialMarkerRegistrationModule::~qSlicerFiducialMarkerRegistrationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFiducialMarkerRegistrationModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFiducialMarkerRegistrationModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFiducialMarkerRegistrationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFiducialMarkerRegistrationModule::icon()const
{
  return QIcon(":/Icons/FiducialMarkerRegistration.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFiducialMarkerRegistrationModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFiducialMarkerRegistrationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFiducialMarkerRegistrationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerFiducialMarkerRegistrationModule
::createWidgetRepresentation()
{
  return new qSlicerFiducialMarkerRegistrationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFiducialMarkerRegistrationModule::createLogic()
{
  return vtkSlicerFiducialMarkerRegistrationLogic::New();
}
