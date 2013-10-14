/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

#include "igstkTracker.h"

#include <QWidget>

class stkPivotCalibrationWidgetPrivate;
class STK_SURGERYNAVIGATION_EXPORT stkPivotCalibrationWidget : public QWidget
{
	Q_OBJECT

public:
	stkPivotCalibrationWidget(QWidget *parent = 0);
	~stkPivotCalibrationWidget();

	//�����������ִ��Pivot�궨�ı任�����͹�����Ϣ
	//���趨λ���Ѿ��ڹ���״̬�У�����������ܻ����2���¼�InitializationSuccessEvent��InitializationFailureEvent
	void RequestInitialize( unsigned int n, igstk::TrackerTool::Pointer trackerTool ); 

	//����������ð�"�궨"��ť�Ϳ�ʼ�ɼ�����֮����¼����
	void RequestSetDelay( unsigned int delayInSeconds );  


	//�������Ҫ�����궨�任
	//�����������һ���ɹ��ı궨�Ժ�ſ��Ա����ã�������2����Ϣ��CoordinateSystemTransformToEvent��TransformNotAvailableEvent
	//�ֱ����һ���궨�任�Ƿ���Ի��
	void RequestCalibrationTransform();


	// �����������Ҫ��pivot�㣬����2����Ϣ��PointEvent��InvalidRequestErrorEvent���ֱ��ʾpivot point�Ƿ���Ի��
	void RequestPivotPoint();

	//���������������Root Mean Square Error (RMSE) ��������2����ϢDoubleTypeEvent��InvalidRequestErrorEvent
	//�ֱ���������RMSE�Ƿ���Ի��
	void RequestCalibrationRMSE();


	/** Add an observer so that an outside class is notified of events. */
	unsigned long AddObserver( const itk::EventObject & event, itk::Command *command );
	unsigned long AddObserver( const itk::EventObject & event, itk::Command *command ) const;
	void RemoveObserver( unsigned long observerID );

signals:
	void setCalibOffset(double,double,double);

protected slots: 
	void on_computeCalibrationButton_clicked();
	void on_completeCalibrationButton_clicked();

protected:
	QScopedPointer<stkPivotCalibrationWidgetPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(stkPivotCalibrationWidget);
	Q_DISABLE_COPY(stkPivotCalibrationWidget);

};
