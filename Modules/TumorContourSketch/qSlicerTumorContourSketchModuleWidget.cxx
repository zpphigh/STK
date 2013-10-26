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
#include <QDebug>

// SlicerQt includes
#include "qSlicerTumorContourSketchModuleWidget.h"
#include "ui_qSlicerTumorContourSketchModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerTumorContourSketchModuleWidgetPrivate: public Ui_qSlicerTumorContourSketchModuleWidget
{
public:
  qSlicerTumorContourSketchModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerTumorContourSketchModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModuleWidgetPrivate::qSlicerTumorContourSketchModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerTumorContourSketchModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModuleWidget::qSlicerTumorContourSketchModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerTumorContourSketchModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerTumorContourSketchModuleWidget::~qSlicerTumorContourSketchModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerTumorContourSketchModuleWidget::setup()
{
  Q_D(qSlicerTumorContourSketchModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
}

