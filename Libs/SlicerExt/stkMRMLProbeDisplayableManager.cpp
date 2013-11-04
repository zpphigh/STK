//链接node和rep

// AnnotationModule/Logic includes
//#include <vtkSlicerMicrowavePlanningLogic.h>

// AnnotationModule/MRML includes
#include <vtkMRMLAnnotationDisplayableManager.h>
#include "vtkMRMLAnnotationDisplayableManagerHelper.h"
#include <vtkMRMLAnnotationLineDisplayNode.h>
#include <vtkMRMLAnnotationNode.h>
#include <vtkMRMLAnnotationPointDisplayNode.h>
#include <stkMRMLProbeNode.h>
#include <vtkMRMLAnnotationTextDisplayNode.h>

// AnnotationModule/MRMLDisplayableManager includes
#include "stkMRMLProbeDisplayableManager.h"

// AnnotationModule/VTKWidgets includes
#include <stkProbeRepresentation3D.h>
#include <vtkDistanceRepresentation.h>
#include <stkProbeWidget.h>
#include <vtkMRMLAnnotationClickCounter.h>

// MRML includes
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLSliceNode.h>
#include "vtkMRMLSelectionNode.h"

// VTK includes
#include <vtkAbstractWidget.h>
#include <vtkAxisActor2D.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkHandleRepresentation.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkLineRepresentation.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkObject.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkProperty2D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkFollower.h>
#include <vtkConeSource.h>
#include <vtkTubeFilter.h>
#include <vtkPlane.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkHandleWidget.h>
#include <QVector3D>
#include <QQuaternion>

// std includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMRMLProbeDisplayableManager);
vtkCxxRevisionMacro (vtkMRMLProbeDisplayableManager, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
// vtkMRMLProbeDisplayableManager Callback
/// \ingroup Slicer_QtModules_Annotation
class vtkProbeWidgetCallback : public vtkCommand
{
public:
  static vtkProbeWidgetCallback *New()
  { return new vtkProbeWidgetCallback; }

  vtkProbeWidgetCallback(){}

  virtual void Execute (vtkObject *vtkNotUsed(caller), unsigned long event, void*)
  {

    if ((event == vtkCommand::EndInteractionEvent) || (event == vtkCommand::InteractionEvent))
      {

      // sanity checks
      if (!this->m_DisplayableManager)
        {
        return;
        }
      if (!this->m_Node)
        {
        return;
        }
      if (!this->m_Widget)
        {
        return;
        }
      // sanity checks end

	  // invoke the selection node event: ActivePlaceNodeIDChangedEvent/ActiveAnnotationIDChangedEvent
	  vtkMRMLSelectionNode* selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->m_Node->GetScene()->GetNthNodeByClass(0, "vtkMRMLSelectionNode"));
	  if (selectionNode)
	  {
		  selectionNode->SetReferenceActivePlaceNodeID(this->m_Node->GetID());
	  }

      if (this->m_DisplayableManager->GetSliceNode())
        {

        // if this is a 2D SliceView displayableManager, restrict the widget to the renderer

        // we need the widgetRepresentation
        vtkProbeRepresentation3D* representation = vtkProbeRepresentation3D::SafeDownCast(this->m_Widget->GetRepresentation());

        double displayCoordinates1[4];
        double displayCoordinates2[4];

        // first, we get the current displayCoordinates of the points
        representation->GetPoint1DisplayPosition(displayCoordinates1);
        representation->GetPoint2DisplayPosition(displayCoordinates2);

        // second, we copy these to restrictedDisplayCoordinates
        double restrictedDisplayCoordinates1[4] = {displayCoordinates1[0], displayCoordinates1[1], displayCoordinates1[2], displayCoordinates1[3]};
        double restrictedDisplayCoordinates2[4] = {displayCoordinates2[0], displayCoordinates2[1], displayCoordinates2[2], displayCoordinates2[3]};

        // modify restrictedDisplayCoordinates 1 and 2, if these are outside the viewport of the current renderer
        this->m_DisplayableManager->RestrictDisplayCoordinatesToViewport(restrictedDisplayCoordinates1);
        this->m_DisplayableManager->RestrictDisplayCoordinatesToViewport(restrictedDisplayCoordinates2);

        // only if we had to restrict the coordinates aka. if the coordinates changed, we update the positions
        if (this->m_DisplayableManager->GetDisplayCoordinatesChanged(displayCoordinates1,restrictedDisplayCoordinates1))
          {
          representation->SetPoint1DisplayPosition(restrictedDisplayCoordinates1);
          }


        if (this->m_DisplayableManager->GetDisplayCoordinatesChanged(displayCoordinates2,restrictedDisplayCoordinates2))
          {
          representation->SetPoint2DisplayPosition(restrictedDisplayCoordinates2);
          }

        }
      if (event == vtkCommand::EndInteractionEvent)
        {
        // save the state of the node when done moving, then call
        // PropagateWidgetToMRML to update the node one last time
        if (this->m_Node->GetScene())
          {
          this->m_Node->GetScene()->SaveStateForUndo(this->m_Node);
          }
        }
      // the interaction with the widget ended, now propagate the changes to MRML
      this->m_DisplayableManager->PropagateWidgetToMRML(this->m_Widget, this->m_Node);

      }
  }

  void SetWidget(vtkAbstractWidget *w)
  {
    this->m_Widget = w;
  }
  void SetNode(vtkMRMLAnnotationNode *n)
  {
    this->m_Node = n;
  }
  void SetDisplayableManager(vtkMRMLAnnotationDisplayableManager * dm)
  {
    this->m_DisplayableManager = dm;
  }

  vtkAbstractWidget * m_Widget;
  vtkMRMLAnnotationNode * m_Node;
  vtkMRMLAnnotationDisplayableManager * m_DisplayableManager;
};

//---------------------------------------------------------------------------
// vtkMRMLProbeDisplayableManager methods
//---------------------------------------------------------------------------
vtkMRMLProbeDisplayableManager::vtkMRMLProbeDisplayableManager()
{
	this->m_Focus="vtkMRMLProbeNode";
	this->plane = vtkPlane::New();
}

//---------------------------------------------------------------------------
vtkMRMLProbeDisplayableManager::~vtkMRMLProbeDisplayableManager()
{
	this->plane->Delete();
}
void vtkMRMLProbeDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
/// Create a new text widget.
vtkAbstractWidget * vtkMRMLProbeDisplayableManager::CreateWidget(vtkMRMLAnnotationNode* node)//OnClickInRenderWindow结束后进入此函数
{

  if (!node)
    {
    vtkErrorMacro("CreateWidget: Node not set!")
    return 0;
    }

  vtkMRMLProbeNode* ProbeNode = vtkMRMLProbeNode::SafeDownCast(node);

  if (!ProbeNode)
    {
    vtkErrorMacro("CreateWidget: Could not get Probe node!")
    return 0;
    }

  vtkProbeWidget * ProbeWidget = vtkProbeWidget::New();

  ProbeWidget->SetInteractor(this->GetInteractor());
  ProbeWidget->SetCurrentRenderer(this->GetRenderer());

  // Create 2D widget
  if (this->GetSliceNode())//设置在2D里的显示
    {
		vtkNew<vtkProbeRepresentation3D> dRep;
		dRep->SetHandleSize(0);
		dRep->SetArrowVisibility(1);
		dRep->InstantiateHandleRepresentation();

		dRep->GetArrowSource()->SetResolution(100);
		dRep->GetArrowSource()->SetAngle(10.0);
		dRep->GetArrowSource()->SetHeight(0.015);
		dRep->GetArrowSource()->SetRadius(0.002);
		dRep->GetTubeSource()->SetRadius(0.002);
		dRep->GetTubeSource()->SetNumberOfSides(6);

		dRep->GetArrow2Source()->SetResolution(100);
		dRep->GetArrow2Source()->SetAngle(10.0);
		dRep->GetArrow2Source()->SetHeight(0.03);
		dRep->GetArrow2Source()->SetRadius(0.006);
		dRep->GetTube2Source()->SetRadius(0.006);
		dRep->GetTube2Source()->SetNumberOfSides(10);

		/*dRep->SetProbeColor(254.0/255.0, 40.0/255.0, 14.0/255.0);*/
		dRep->SetSelectedProbeColor(255.0/255.0, 0.0/255.0, 0.0/255.0);

		ProbeWidget->SetRepresentation(dRep.GetPointer());
		bool current = ProbeNode->GetCurrent();
		bool visible = ProbeNode->GetVisibleIn2DWidget();
		if (!(visible&current))
		{
			dRep->SetVisibility(false);
		}
    }
  else
    {
    vtkNew<vtkProbeRepresentation3D> dRep2;
	dRep2->SetHandleSize(0);
	dRep2->SetArrowVisibility(1);
	dRep2->InstantiateHandleRepresentation();

	/*dRep2->SetProbeColor(254.0/255.0, 40.0/255.0, 14.0/255.0);*/
	dRep2->SetSelectedProbeColor(255.0/255.0, 0.0/255.0, 0.0/255.0);

    ProbeWidget->SetRepresentation(dRep2.GetPointer());
    ProbeWidget->On();
    }

  this->PropagateMRMLToWidget(ProbeNode, ProbeWidget);//更新MRML到Widget的信息

  //vtkDebugMacro("CreateWidget: Widget was set up")

  return ProbeWidget;

}

//---------------------------------------------------------------------------
/// Tear down the widget creation
void vtkMRMLProbeDisplayableManager::OnWidgetCreated(vtkAbstractWidget * widget, vtkMRMLAnnotationNode * node)//没有设置
{

  if (!widget)
    {
    vtkErrorMacro("OnWidgetCreated: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("OnWidgetCreated: MRML node was null!")
    return;
    }

  vtkProbeWidget * ProbeWidget = vtkProbeWidget::SafeDownCast(widget);

  if (!widget)
    {
    vtkErrorMacro("OnWidgetCreated: Could not get Probe widget")
    return;
    }

  vtkMRMLProbeNode * ProbeNode = vtkMRMLProbeNode::SafeDownCast(node);

  if (!ProbeNode)
    {
    vtkErrorMacro("OnWidgetCreated: Could not get ProbeNode node")
    return;
    }

  bool widgetState = ProbeNode->GetFocalLocked();
  ProbeWidget->SetEnabled(~widgetState);  // 如果probe的FocalLocked为true，说明probe为navigation probe，不响应鼠标交互事件

  // add observer for end interaction
  vtkProbeWidgetCallback *myCallback = vtkProbeWidgetCallback::New();
  myCallback->SetNode(node);
  myCallback->SetWidget(widget);
  myCallback->SetDisplayableManager(this);
  widget->AddObserver(vtkCommand::EndInteractionEvent,myCallback);
  widget->AddObserver(vtkCommand::InteractionEvent,myCallback);
  myCallback->Delete();

  node->SaveView();
}

//---------------------------------------------------------------------------
/// Propagate properties of MRML node to widget.
void vtkMRMLProbeDisplayableManager::PropagateMRMLToWidget(vtkMRMLAnnotationNode* node, vtkAbstractWidget * widget)
{
  if (!widget)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("PropagateMRMLToWidget: MRML node was null!")
    return;
    }

  // cast to the specific widget
  vtkProbeWidget* ProbeWidget = vtkProbeWidget::SafeDownCast(widget);

  if (!ProbeWidget)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get Probe widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLProbeNode* ProbeNode = vtkMRMLProbeNode::SafeDownCast(node);

  if (!ProbeNode)
    {
    vtkErrorMacro("PropagateMRMLToWidget: Could not get Probe node!")
    return;
    }

  // disable processing of modified events
  this->m_Updating = 1;
  
  vtkMRMLAnnotationTextDisplayNode *textDisplayNode = ProbeNode->GetAnnotationTextDisplayNode();
  vtkMRMLAnnotationPointDisplayNode *pointDisplayNode = ProbeNode->GetAnnotationPointDisplayNode();
  vtkMRMLAnnotationLineDisplayNode *lineDisplayNode = ProbeNode->GetAnnotationLineDisplayNode();

  // update the location
  if (this->Is2DDisplayableManager())
    {

    // now get the widget properties (coordinates, measurement etc.) and if the mrml node has changed, propagate the changes
    vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());

	rep->NeedToRenderOn();
	ProbeWidget->On();
    }
  else
    {
    /// 3d case
    // now get the widget properties (coordinates, measurement etc.) and if the mrml node has changed, propagate the changes
    vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());

    if (pointDisplayNode)
      {
      // use this scale for the ticks
      rep->SetHandleSize(pointDisplayNode->GetGlyphScale());
      }
    if (lineDisplayNode)
      {
      if (ProbeNode->GetSelected())
        {
        rep->GetLineHandleRepresentation()->GetProperty()->SetColor(lineDisplayNode->GetSelectedColor());
        }
      else
        {
        rep->GetLineHandleRepresentation()->GetProperty()->SetColor(lineDisplayNode->GetColor());
        }
      if (rep->GetLineProperty())
        {
        if (ProbeNode->GetSelected())
          {
          rep->GetLineProperty()->SetColor(lineDisplayNode->GetSelectedColor());
          }
        else
          {
          rep->GetLineProperty()->SetColor(lineDisplayNode->GetColor());
          }
        rep->GetLineProperty()->SetLineWidth(lineDisplayNode->GetLineThickness());
        rep->GetLineProperty()->SetOpacity(lineDisplayNode->GetOpacity());
	    }
      }

    rep->NeedToRenderOn();
    }

  // update the position
  this->UpdatePosition(widget, node);

  ProbeWidget->Modified();//这个函数很重要，会触发callback，刷新四个窗口

  // enable processing of modified events
  this->m_Updating = 0;

}

//---------------------------------------------------------------------------
/// Propagate properties of widget to MRML node.
void vtkMRMLProbeDisplayableManager::PropagateWidgetToMRML(vtkAbstractWidget * widget, vtkMRMLAnnotationNode* node)//没有设置显示的语句
{

  if (!widget)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("PropagateWidgetToMRML: MRML node was null!")
    return;
    }

  // cast to the specific widget
  vtkProbeWidget* ProbeWidget = vtkProbeWidget::SafeDownCast(widget);

  if (!ProbeWidget)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Could not get Probe widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLProbeNode* ProbeNode = vtkMRMLProbeNode::SafeDownCast(node);

  if (!ProbeNode)
    {
    vtkErrorMacro("PropagateWidgetToMRML: Could not get Probe node!")
    return;
    }

  // disable processing of modified events
  this->m_Updating = 1;
  ProbeNode->DisableModifiedEventOn();

  double header[4] = {0, 0, 0, 1};
  ProbeNode->GetHeader(header);

  // 初始方向向量
  double probeDirection[3];
  QVector3D v0;
  ProbeNode->GetNormal(probeDirection);
  v0.setX(probeDirection[0]);
  v0.setY(probeDirection[1]);
  v0.setZ(probeDirection[2]);

  double endPoint[4] = {0, 0, 0, 1};
  endPoint[0] = header[0] + probeDirection[0] * ProbeNode->GetLength();
  endPoint[1] = header[1] + probeDirection[1] * ProbeNode->GetLength();
  endPoint[2] = header[2] + probeDirection[2] * ProbeNode->GetLength();

  // 用来存放移动位置后的probe的两个端点坐标;
  double worldCoordinates1[4]={0,0,0,1};
  double worldCoordinates2[4]={0,0,0,1};

  bool allowMovement = true;

  if (this->GetSliceNode())
    {
    // 2D widget was changed
    // now get the widget properties (coordinates, measurement etc.) and save it to the mrml node
    vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());

    double displayCoordinates1[4] = {0, 0, 0, 1};
    double displayCoordinates2[4] = {0, 0, 0, 1};

	// 提取rep中保存的交互后的probe坐标信息
    rep->GetPoint1DisplayPosition(displayCoordinates1); 
    rep->GetPoint2DisplayPosition(displayCoordinates2); // 此处计算的displayCoordinates中，第三维的大小都是0;

	double tempDisplayCoordinates[4] = {0, 0, 0, 1};
	this->GetWorldToDisplayCoordinates(header, tempDisplayCoordinates);

	tempDisplayCoordinates[0] = displayCoordinates1[0];
	tempDisplayCoordinates[1] = displayCoordinates1[1];

	this->GetDisplayToWorldCoordinates(tempDisplayCoordinates, worldCoordinates1);

	// 获取由rep获取的probe在2D窗口投影面上的world坐标信息
	double worldCoordinatesBuffer1[4] = {0, 0, 0, 1};
	double worldCoordinatesBuffer2[4] = {0, 0, 0, 1};
	this->GetDisplayToWorldCoordinates(displayCoordinates1, worldCoordinatesBuffer1);
	this->GetDisplayToWorldCoordinates(displayCoordinates2, worldCoordinatesBuffer2);


	// probe中保存的交互前的位置信息，投影到2D窗口所在的平面
	double worldCoordinatesProject1[4] = {0, 0, 0, 1};
	double worldCoordinatesProject2[4] = {0, 0, 0, 1};

	vtkMatrix4x4* sliceToRAS = this->GetSliceNode()->GetSliceToRAS();

	QVector3D planeNormal;
	planeNormal.setX(sliceToRAS->GetElement(0, 2));
	planeNormal.setY(sliceToRAS->GetElement(1, 2));
	planeNormal.setZ(sliceToRAS->GetElement(2, 2));
	planeNormal.normalize();

	this->plane->SetNormal(sliceToRAS->GetElement(0, 2),
						   sliceToRAS->GetElement(1, 2),
						   sliceToRAS->GetElement(2, 2));
	this->plane->SetOrigin(sliceToRAS->GetElement(0, 3),
						   sliceToRAS->GetElement(1, 3),
						   sliceToRAS->GetElement(2, 3));
	plane->GeneralizedProjectPoint(header, worldCoordinatesProject1);   // probe中存的旧坐标点的投影;
	plane->GeneralizedProjectPoint(endPoint, worldCoordinatesProject2);

	// probe中保存的交互前位置投影在2D平面上的方向
	double probeDirection2D[3] = {0, 0, 0};
	double probeLength2D = sqrt(vtkMath::Distance2BetweenPoints(worldCoordinatesProject1, worldCoordinatesProject2));
	probeDirection2D[0] = (worldCoordinatesProject1[0] - worldCoordinatesProject2[0])/probeLength2D;
	probeDirection2D[1] = (worldCoordinatesProject1[1] - worldCoordinatesProject2[1])/probeLength2D;
	probeDirection2D[2] = (worldCoordinatesProject1[2] - worldCoordinatesProject2[2])/probeLength2D;

	int state = rep->GetInteractionState();
	if (state == vtkProbeRepresentation3D::TranslatingP1)  // P1点有位移;
	{
		// 针头handle沿着投影方向在2D平面上移动
		double t, closestP[4]={0, 0, 0, 1};
		vtkLine::DistanceToLine(worldCoordinatesBuffer1, worldCoordinatesProject1, worldCoordinatesProject2, t, closestP);

		closestP[0] = worldCoordinatesProject1[0] + t * (worldCoordinatesProject2[0] - worldCoordinatesProject1[0]);
		closestP[1] = worldCoordinatesProject1[1] + t * (worldCoordinatesProject2[1] - worldCoordinatesProject1[1]);
		closestP[2] = worldCoordinatesProject1[2] + t * (worldCoordinatesProject2[2] - worldCoordinatesProject1[2]);

		// 根据投影的规律，向量(worldCoordinatesProject1,header) == 向量(closestP,worldCoordinates1)
		worldCoordinates1[0] = closestP[0] + header[0] - worldCoordinatesProject1[0];
		worldCoordinates1[1] = closestP[1] + header[1] - worldCoordinatesProject1[1];
		worldCoordinates1[2] = closestP[2] + header[2] - worldCoordinatesProject1[2];
	}
	else if (state == vtkProbeRepresentation3D::TranslatingP2)// p1点不动，p2点有位移;
	{
		// 计算probe在2D窗口投影的新方向，worldCoordinatesBuffers坐标组成的向量
        double newDirection2D[3] = {0, 0, 0};
		double tempLength = sqrt(vtkMath::Distance2BetweenPoints(worldCoordinatesBuffer1, worldCoordinatesBuffer2));
		newDirection2D[0] = (worldCoordinatesBuffer1[0] - worldCoordinatesBuffer2[0])/tempLength;
		newDirection2D[1] = (worldCoordinatesBuffer1[1] - worldCoordinatesBuffer2[1])/tempLength;
		newDirection2D[2] = (worldCoordinatesBuffer1[2] - worldCoordinatesBuffer2[2])/tempLength;

		// 计算probe在2D窗口投影交互前后的方向向量之间夹角theta
		double dot = vtkMath::Dot(newDirection2D, probeDirection2D);
		double theta = vtkMath::DegreesFromRadians(acos(dot));
		theta = -theta;

		// 利用旋转后的probe与原probe方向叉乘结果，判断probe是逆时针或者顺时针转动
		double cross[3] = {0, 0, 0};
		vtkMath::Cross(probeDirection2D, newDirection2D, cross); // 两方向向量的叉乘结果
		QVector3D croProduct;
		croProduct.setX(cross[0]);
		croProduct.setY(cross[1]);
		croProduct.setZ(cross[2]);
		croProduct.normalize();  // 单位化

		QVector3D sub;
		sub = croProduct - planeNormal;

		double l = sub.length();  // 两向量相减，求结果向量的长度，如果两向量相等，差的模接近于零
		if (l < 0.03)
		{
			theta = -theta;  // 如果叉乘向量与平面法向量相反，旋转角度取负，反之旋转角度为正
		}

		// 由quartenion计算绕着窗口平面法向向量旋转角度theta之后，原probe的方向
		QQuaternion rotate = QQuaternion::fromAxisAndAngle(planeNormal,theta);

		QVector3D newNormal = rotate.rotatedVector(v0);
		ProbeNode->GetOrientation()->setVector(newNormal);
		
		probeDirection[0] = newNormal.x();
		probeDirection[1] = newNormal.y();
		probeDirection[2] = newNormal.z();
		ProbeNode->SetNormal(probeDirection[0], probeDirection[1], probeDirection[2]);
	}

    if (displayCoordinates1[0] < 0 || displayCoordinates1[0] > this->GetInteractor()->GetRenderWindow()->GetSize()[0])
      {
      allowMovement = false;
      }

    if (displayCoordinates1[1] < 0 || displayCoordinates1[1] > this->GetInteractor()->GetRenderWindow()->GetSize()[1])
      {
      allowMovement = false;
      }

    if (displayCoordinates2[0] < 0 || displayCoordinates2[0] > this->GetInteractor()->GetRenderWindow()->GetSize()[0])
      {
      allowMovement = false;
      }

    if (displayCoordinates2[1] < 0 || displayCoordinates2[1] > this->GetInteractor()->GetRenderWindow()->GetSize()[1])
      {
      allowMovement = false;
      }
    }
  else
    {
    // now get the widget properties (coordinates, measurement etc.) and save it to the mrml node
    vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());

    rep->GetPoint1WorldPosition(worldCoordinates1);
    rep->GetPoint2WorldPosition(worldCoordinates2);
	
    // save worldCoordinates to MRML if changed

	double endPoint[4] = {0, 0, 0, 1};

	endPoint[0] = header[0] + ProbeNode->GetLength() * probeDirection[0];
	endPoint[1] = header[1] + ProbeNode->GetLength() * probeDirection[1];
	endPoint[2] = header[2] + ProbeNode->GetLength() * probeDirection[2];

	int state = rep->GetInteractionState();

	if (state == vtkProbeRepresentation3D::TranslatingP1)
	{
		// 拖动针尖进动，更新针尖的值，但是方向不变;
		// 求出现在的worldCoordinates1在Probe所在的直线上的投影，并将投影点设为P2的新位置;

		double t, closestP[4]={0, 0, 0, 1};
		vtkLine::DistanceToLine(worldCoordinates1, header, endPoint, t, closestP);

		closestP[0] = header[0] + t * (endPoint[0] - header[0]);
		closestP[1] = header[1] + t * (endPoint[1] - header[1]);
		closestP[2] = header[2] + t * (endPoint[2] - header[2]);

		worldCoordinates1[0] = closestP[0];
		worldCoordinates1[1] = closestP[1];
		worldCoordinates1[2] = closestP[2];
	}
	else if (state == vtkProbeRepresentation3D::TranslatingP2)  // p2点的位置有移动;
	{
		// 拖动p1会使针尾转动, 转动针尾，probe的方向改变，针尖位置不变，所以只更新方向;

		QVector3D repNormal;
		repNormal.setX(worldCoordinates2[0] - worldCoordinates1[0]);
		repNormal.setY(worldCoordinates2[1] - worldCoordinates1[1]);
		repNormal.setZ(worldCoordinates2[2] - worldCoordinates1[2]);
		repNormal.normalize();

		double norm[3] = {0, 0, 0};
		norm[0] = repNormal.x();
		norm[1] = repNormal.y();
		norm[2] = repNormal.z();

		ProbeNode->SetNormal(norm[0],norm[1],norm[2]);
		ProbeNode->GetOrientation()->setVector(repNormal);
	}

    }

  // if movement is not allowed, jump out
  if (!allowMovement)
    {
    return;
    }

  // save worldCoordinates to MRML if changed

  if (this->GetWorldCoordinatesChanged(worldCoordinates1, header))
  {
	  ProbeNode->SetPosition(worldCoordinates1);
  }

  // save the current view
  ProbeNode->SaveView();

  // enable processing of modified events
  ProbeNode->DisableModifiedEventOff();

  ProbeNode->Modified();
  ProbeNode->GetScene()->InvokeEvent(vtkCommand::ModifiedEvent, ProbeNode);

  // This displayableManager should now consider ModifiedEvent again
  this->m_Updating = 0;
}

//---------------------------------------------------------------------------
/// Create a AnnotationMRMLnode
void vtkMRMLProbeDisplayableManager::OnClickInRenderWindow(double x, double y, const char *associatedNodeID)//创建probeNode，无关显示
{
	if (!this->IsCorrectDisplayableManager())
	{
		// jump out
		return;
	}

	double X, Y;
	X = this->GetInteractor()->GetEventPosition()[0];
	Y = this->GetInteractor()->GetEventPosition()[1];

	double worldCoordinates1[4] = {0, 0, 0, 1};
	this->GetDisplayToWorldCoordinates(X, Y, worldCoordinates1);

	vtkMRMLProbeNode *ProbeNode = vtkMRMLProbeNode::New();
	ProbeNode->SetPosition(worldCoordinates1);

	// switch to updating state to avoid events mess
	this->m_Updating = 1;

	QVector3D probeOrient;
	probeOrient.setX(-1.0);
	probeOrient.setY(0.0);
	probeOrient.setZ(0.0);
	probeOrient.normalize(); // 单位方向向量;

	// 计算Quaternion, 当前Quartenion为针的初始方向，所以quartenion表示形式为标量部分为0，向量部分为针的方向向量;
	double theata = 0.0;

	QQuaternion* orient = new QQuaternion;
	orient->setVector(probeOrient);
	orient->setScalar(0);

	ProbeNode->SetOrientation(orient);

    // if this was a one time place, go back to view transform mode
    vtkMRMLInteractionNode *interactionNode = this->GetInteractionNode();
    if (interactionNode && interactionNode->GetPlaceModePersistence() != 1)
      {
      interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
      }

    this->GetMRMLScene()->SaveStateForUndo();

    // is there a node associated with this?
    if (associatedNodeID)
      {
		  ProbeNode->SetAttribute("AssociatedNodeID", associatedNodeID);
      }

    ProbeNode->Initialize(this->GetMRMLScene());

    ProbeNode->Delete();

    // reset updating state
    this->m_Updating = 0;
    
  }

//---------------------------------------------------------------------------
void vtkMRMLProbeDisplayableManager::UpdatePosition(vtkAbstractWidget *widget, vtkMRMLNode *node)
{
    if (!widget)
    {
    vtkErrorMacro("UpdatePosition: Widget was null!")
    return;
    }

  if (!node)
    {
    vtkErrorMacro("UpdatePosition: MRML node was null!")
    return;
    }

  // cast to the specific widget
  vtkProbeWidget* ProbeWidget = vtkProbeWidget::SafeDownCast(widget);

  if (!ProbeWidget)
    {
    vtkErrorMacro("UpdatePosition: Could not get Probe widget!")
    return;
    }

  // cast to the specific mrml node
  vtkMRMLProbeNode* ProbeNode = vtkMRMLProbeNode::SafeDownCast(node);

  if (!ProbeNode)
    {
    vtkErrorMacro("UpdatePosition: Could not get Probe node!")
    return;
    }

  // disable processing of modified events
  this->m_Updating = 1;

  // wordCoordinates1 和 worldCoordiantes2 分别存放针尖和针尾坐标
  double worldCoordinates1[4]={0,0,0,1};
  double worldCoordinates2[4]={0,0,0,1};
  double normal[3] = {0, 0, 0};

  ProbeNode->GetHeader(worldCoordinates1);
  ProbeNode->GetNormal(normal);

  worldCoordinates2[0] = worldCoordinates1[0] + ProbeNode->GetLength() * normal[0];
  worldCoordinates2[1] = worldCoordinates1[1] + ProbeNode->GetLength() * normal[1];
  worldCoordinates2[2] = worldCoordinates1[2] + ProbeNode->GetLength() * normal[2];

  double color[3] = {0,0,0};
  ProbeNode->GetColor(color);

  // update the location
  if (this->Is2DDisplayableManager())
    {
		// get the 2d representation
		vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());

		bool colorChanged = ProbeNode->GetColorChanged();
		if (colorChanged)
		{
			rep->SetProbeColor(color[0], color[1], color[2]);
		}

	// 先将当前slice移动到针尖所在的平面;
    bool FocalLocked = ProbeNode->GetFocalLocked();
	bool current = ProbeNode->GetCurrent();

	vtkMRMLSliceNode* slice = this->GetSliceNode();
	if (!slice)
	{
		return;
	}

	if ((!FocalLocked)&current)
	{
		slice->JumpSlice(worldCoordinates1[0], worldCoordinates1[1], worldCoordinates1[2]);//三个slice的交点在针尖
	}

	// get the origin and normal of this 2D plane
	vtkMatrix4x4* sliceToRAS = slice->GetSliceToRAS();
	this->plane->SetNormal(sliceToRAS->GetElement(0,2),
		sliceToRAS->GetElement(1,2),
		sliceToRAS->GetElement(2,2));
	this->plane->SetOrigin(sliceToRAS->GetElement(0,3),
		sliceToRAS->GetElement(1,3),
		sliceToRAS->GetElement(2,3));

	// compute the project of two handle world coordinates in this 2D plane
	double worldCoordinatesProject1[4] = {0, 0, 0, 1};
	double worldCoordinatesProject2[4] = {0, 0, 0, 1};

	plane->GeneralizedProjectPoint(worldCoordinates1, worldCoordinatesProject1);
	plane->GeneralizedProjectPoint(worldCoordinates2, worldCoordinatesProject2);

    // change the 2D location
	// get the project point display coordinates and compare to previous display coordinates
    double displayCoordinates1[4]={0,0,0,1};
    double displayCoordinates2[4]={0,0,0,1};
    double displayCoordinatesBuffer1[4]={0,0,0,1};
    double displayCoordinatesBuffer2[4]={0,0,0,1};

    this->GetWorldToDisplayCoordinates(worldCoordinatesProject1,displayCoordinates1);
    this->GetWorldToDisplayCoordinates(worldCoordinatesProject2,displayCoordinates2);

    // only update the position, if coordinates really change
    rep->GetPoint1DisplayPosition(displayCoordinatesBuffer1);
    rep->GetPoint2DisplayPosition(displayCoordinatesBuffer2);

    if (this->GetDisplayCoordinatesChanged(displayCoordinates1,displayCoordinatesBuffer1))
      {
      rep->SetPoint1DisplayPosition(displayCoordinates1);
      }
    if (this->GetDisplayCoordinatesChanged(displayCoordinates2,displayCoordinatesBuffer2))
      {
      rep->SetPoint2DisplayPosition(displayCoordinates2);
      }

	bool visible = ProbeNode->GetVisibleIn2DWidget();
	if (current|visible)
	{
		rep->SetVisibility(true);
	}
	else
	{
		rep->SetVisibility(false);
	}
    }
  else
    {
    /// 3d case
    // now get the widget properties (coordinates, measurement etc.) and if the mrml node has changed, propagate the changes
    vtkProbeRepresentation3D * rep = vtkProbeRepresentation3D::SafeDownCast(ProbeWidget->GetRepresentation());
    // change the 3D location
    rep->SetPoint1WorldPosition(worldCoordinates1);
    rep->SetPoint2WorldPosition(worldCoordinates2);

	rep->SetProbeColor(color[0], color[1], color[2]);

	bool current = ProbeNode->GetCurrent();
	bool visible = ProbeNode->GetVisibleIn3DWidget();

	if (current|visible)
	{
		rep->SetVisibility(true);
	}
	else
	{
		rep->SetVisibility(false);
	}
    rep->NeedToRenderOn();
    }

  ProbeWidget->Modified();

  // enable processing of modified events
  this->m_Updating = 0;
}


