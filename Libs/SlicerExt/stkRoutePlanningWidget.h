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

	// ���ݵ�ǰannotation�����ƣ��ж���Ӧ�İ�ť�Ƿ���checked״̬;
	void onActivePlaceNodeIDChanged();
	void onActivePlaceNodeNameChanged(const char* id);

protected:
  QScopedPointer<stkRoutePlanningWidgetPrivate> d_ptr;
  
  void UpdateProbeTabelFromMRML();

  // ����MRML�Ľ���״̬���ж���Ӧ��ť�Ƿ���checked״̬;
  void updateWidgetFromMRML();

  void AddRow();

private:
  Q_DECLARE_PRIVATE(stkRoutePlanningWidget);
  Q_DISABLE_COPY(stkRoutePlanningWidget);

  std::string CurrentProbeNode;
};
