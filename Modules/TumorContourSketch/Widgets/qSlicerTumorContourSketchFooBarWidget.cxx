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
#include "qSlicerTumorContourSketchFooBarWidget.h"
#include "ui_qSlicerTumorContourSketchFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_TumorContourSketch
class qSlicerTumorContourSketchFooBarWidgetPrivate
  : public Ui_qSlicerTumorContourSketchFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerTumorContourSketchFooBarWidget);
protected:
  qSlicerTumorContourSketchFooBarWidget* const q_ptr;

public:
  qSlicerTumorContourSketchFooBarWidgetPrivate(
    qSlicerTumorContourSketchFooBarWidget& object);
  virtual void setupUi(qSlicerTumorContourSketchFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerTumorContourSketchFooBarWidgetPrivate
::qSlicerTumorContourSketchFooBarWidgetPrivate(
  qSlicerTumorContourSketchFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerTumorContourSketchFooBarWidgetPrivate
::setupUi(qSlicerTumorContourSketchFooBarWidget* widget)
{
  this->Ui_qSlicerTumorContourSketchFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerTumorContourSketchFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchFooBarWidget
::qSlicerTumorContourSketchFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerTumorContourSketchFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerTumorContourSketchFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchFooBarWidget
::~qSlicerTumorContourSketchFooBarWidget()
{
}
