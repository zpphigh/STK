/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerSurgeryRoutePlanningFooBarWidget.h"
#include "ui_qSlicerSurgeryRoutePlanningFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SurgeryRoutePlanning
class qSlicerSurgeryRoutePlanningFooBarWidgetPrivate
  : public Ui_qSlicerSurgeryRoutePlanningFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerSurgeryRoutePlanningFooBarWidget);
protected:
  qSlicerSurgeryRoutePlanningFooBarWidget* const q_ptr;

public:
  qSlicerSurgeryRoutePlanningFooBarWidgetPrivate(
    qSlicerSurgeryRoutePlanningFooBarWidget& object);
  virtual void setupUi(qSlicerSurgeryRoutePlanningFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningFooBarWidgetPrivate
::qSlicerSurgeryRoutePlanningFooBarWidgetPrivate(
  qSlicerSurgeryRoutePlanningFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerSurgeryRoutePlanningFooBarWidgetPrivate
::setupUi(qSlicerSurgeryRoutePlanningFooBarWidget* widget)
{
  this->Ui_qSlicerSurgeryRoutePlanningFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerSurgeryRoutePlanningFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningFooBarWidget
::qSlicerSurgeryRoutePlanningFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerSurgeryRoutePlanningFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerSurgeryRoutePlanningFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerSurgeryRoutePlanningFooBarWidget
::~qSlicerSurgeryRoutePlanningFooBarWidget()
{
}
