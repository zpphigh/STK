#pragma once

// STK includes

#include "qSlicerWidget.h"

#include "stkSlicerExtExport.h"
#include <QModelIndex>

class stkRoutePlanningWidgetPrivate;

class STK_SLICEREXT_EXPORT stkRoutePlanningWidget : public qSlicerWidget
{
  Q_OBJECT

public:

  typedef qSlicerWidget Superclass;
  stkRoutePlanningWidget(QWidget *parent=0);
  virtual ~stkRoutePlanningWidget();

  std::string GetCurrentProbeNode(){return this->CurrentProbeNode;}
  void SetCurrentProbeNode(std::string probe);

  
public slots:
	void on_addProbeButton_clicked();
	void on_deleteProbeButton_Clicked();
	void on_clearProbesButton_Clicked();

	void onProbeTableRowSelected(const QModelIndex&);
	void onNodeAdded(vtkObject*, vtkObject*);

	// 根据当前annotation的名称，判断相应的按钮是否处于checked状态;
	void onActivePlaceNodeIDChanged();
	void onActivePlaceNodeNameChanged(const char* id);

protected:
  QScopedPointer<stkRoutePlanningWidgetPrivate> d_ptr;
  
  void UpdateProbeTabelFromMRML();

  // 根据MRML的交互状态，判断相应按钮是否处于checked状态;
  void updateWidgetFromMRML();

  void AddRow();

private:
  Q_DECLARE_PRIVATE(stkRoutePlanningWidget);
  Q_DISABLE_COPY(stkRoutePlanningWidget);

  std::string CurrentProbeNode;
};
