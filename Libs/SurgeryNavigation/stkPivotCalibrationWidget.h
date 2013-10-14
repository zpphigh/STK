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

	//这个方法设置执行Pivot标定的变换数量和工具信息
	//假设定位器已经在工作状态中，这个方法可能会产生2个事件InitializationSuccessEvent和InitializationFailureEvent
	void RequestInitialize( unsigned int n, igstk::TrackerTool::Pointer trackerTool ); 

	//这个方法设置按"标定"按钮和开始采集数据之间的事件间隔
	void RequestSetDelay( unsigned int delayInSeconds );  


	//这个方法要求计算标定变换
	//这个方法尽在一个成功的标定以后才可以被调用，它产生2个消息：CoordinateSystemTransformToEvent和TransformNotAvailableEvent
	//分别标明一个标定变换是否可以获得
	void RequestCalibrationTransform();


	// 这个方法用来要求pivot点，生成2个消息：PointEvent和InvalidRequestErrorEvent，分别表示pivot point是否可以获得
	void RequestPivotPoint();

	//这个方法用来计算Root Mean Square Error (RMSE) ，它生成2个消息DoubleTypeEvent和InvalidRequestErrorEvent
	//分别用来标明RMSE是否可以获得
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
