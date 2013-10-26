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

// SurgeryRoutePlanning Logic includes
#include <vtkSlicerSurgeryRoutePlanningLogic.h>

// SurgeryRoutePlanning includes
#include "qSlicerSurgeryRoutePlanningModule.h"
#include "qSlicerSurgeryRoutePlanningModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerSurgeryRoutePlanningModule, qSlicerSurgeryRoutePlanningModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerSurgeryRoutePlanningModulePrivate
{
public:
  qSlicerSurgeryRoutePlanningModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerSurgeryRoutePlanningModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningModulePrivate
::qSlicerSurgeryRoutePlanningModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerSurgeryRoutePlanningModule methods

//-----------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningModule
::qSlicerSurgeryRoutePlanningModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerSurgeryRoutePlanningModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningModule::~qSlicerSurgeryRoutePlanningModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerSurgeryRoutePlanningModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerSurgeryRoutePlanningModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSurgeryRoutePlanningModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerSurgeryRoutePlanningModule::icon()const
{
  return QIcon(":/Icons/SurgeryRoutePlanning.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerSurgeryRoutePlanningModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSurgeryRoutePlanningModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerSurgeryRoutePlanningModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerSurgeryRoutePlanningModule
::createWidgetRepresentation()
{
  return new qSlicerSurgeryRoutePlanningModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerSurgeryRoutePlanningModule::createLogic()
{
  return vtkSlicerSurgeryRoutePlanningLogic::New();
}
