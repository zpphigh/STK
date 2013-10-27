// OpenIGTLinkIF MRML includes
#include "stkIGTLCircularBuffer.h"
#include "stkMRMLIGTLServerNode.h"

// OpenIGTLink includes
#include <igtlServerSocket.h>
#include <igtlClientSocket.h>
#include <igtlOSUtil.h>
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkCollection.h>
#include <vtkEventBroker.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMultiThreader.h>
#include <vtkMutexLock.h>
#include <vtkObjectFactory.h>

// STD includes
#include <string>
#include <iostream>
#include <sstream>
#include <map>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(stkMRMLIGTLServerNode);

//----------------------------------------------------------------------------
const char *stkMRMLIGTLServerNode::ConnectorStateStr[stkMRMLIGTLServerNode::NUM_STATE] =
{
  "OFF",       // OFF
  "WAIT",      // WAIT_CONNECTION
  "ON",        // CONNECTED
};

//----------------------------------------------------------------------------
stkMRMLIGTLServerNode::stkMRMLIGTLServerNode()
{
  this->HideFromEditors = false;

  this->State  = STATE_OFF;

  this->Thread = vtkMultiThreader::New();
  this->ServerStopFlag = false;
  this->ThreadID = -1;
  this->ServerPort = 18944;
  this->Mutex = vtkMutexLock::New();
  this->CircularBufferMutex = vtkMutexLock::New();

  this->EventQueueMutex = vtkMutexLock::New();

  this->IncomingDeviceIDSet.clear();
  this->OutgoingDeviceIDSet.clear();
  this->UnspecifiedDeviceIDSet.clear();

  this->MessageConverterList.clear();
  this->IGTLNameToConverterMap.clear();
  this->MRMLIDToConverterMap.clear();

  this->OutgoingMRMLNodeList.clear();
  this->IncomingMRMLNodeInfoList.clear();

  this->CheckCRC = 1;
}

//----------------------------------------------------------------------------
stkMRMLIGTLServerNode::~stkMRMLIGTLServerNode()
{
  this->Stop();
  if (this->Thread)
    {
    this->Thread->Delete();
    }

  if (this->Mutex)
    {
    this->Mutex->Delete();
    }

  this->CircularBufferMutex->Lock();
  CircularBufferMap::iterator iter;
  for (iter = this->Buffer.begin(); iter != this->Buffer.end(); iter ++)
    {
    iter->second->Delete();
    }
  this->Buffer.clear();
  this->CircularBufferMutex->Unlock();

  if (this->CircularBufferMutex)
    {
    this->CircularBufferMutex->Delete();
    }

  if (this->EventQueueMutex)
    {
    this->EventQueueMutex->Delete();
    }

}


//----------------------------------------------------------------------------
void stkMRMLIGTLServerNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  of << " connectorType=\"" << "SERVER" << "\" ";
  of << " serverPort=\"" << this->ServerPort << "\" ";

}


//----------------------------------------------------------------------------
void stkMRMLIGTLServerNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  const char* serverHostname = "";
  int port = 0;
  int type = -1;
  int restrictDeviceName = 0;

  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "connectorType"))
      {
      }
    if (!strcmp(attName, "serverHostname"))
      {
      serverHostname = attValue;
      }
    if (!strcmp(attName, "serverPort"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> port;
      }
    }

  SetServerPort(port);
}



//----------------------------------------------------------------------------
void stkMRMLIGTLServerNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  MRMLNodeListType::iterator iter;
  vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

  for (iter = this->OutgoingMRMLNodeList.begin(); iter != this->OutgoingMRMLNodeList.end(); iter ++)
    {
    if (node == *iter)
      {
      int size;
      void* igtlMsg;
      stkIGTLToMRMLBase* converter = this->MRMLIDToConverterMap[node->GetID()];
      if (converter->MRMLToIGTL(event, node, &size, &igtlMsg))
        {
        int r = this->SendData(size, (unsigned char*)igtlMsg);
        if (r == 0)
          {
          // TODO: error handling
          //std::cerr << "ERROR: send data." << std::endl;
          }
        return;
        }
      }
    }
}

//----------------------------------------------------------------------------
void stkMRMLIGTLServerNode::SetCheckCRC(int c)
{
  if (c == 0)
    {
    this->CheckCRC = 0;
    }
  else
    {
    this->CheckCRC = 1;
    }

  // Set CheckCRC flag in each converter
  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    (*iter)->SetCheckCRC(this->CheckCRC);
    }

}

//---------------------------------------------------------------------------
int stkMRMLIGTLServerNode::Start()
{
  //  如果线程已经存在则直接返回
  if (this->ThreadID >= 0)
    return 0;

  //设置标志位，从而控制线程里面的while循环的进行
  this->ServerStopFlag = false;

  //Thread变量一直为本Node所有，并且记录其ID号
  this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &stkMRMLIGTLServerNode::ThreadFunction, this);

  // Following line is necessary in some Linux environment,
  // since it takes for a while for the thread to update
  // this->State to non STATE_OFF value. This causes error
  // after calling stkMRMLIGTLServerNode::Start() in ProcessGUIEvent()
  // in vtkOpenIGTLinkIFGUI class.
  this->State = STATE_WAIT_CONNECTION;
  this->InvokeEvent(stkMRMLIGTLServerNode::ActivatedEvent);

  return 1;
}

//---------------------------------------------------------------------------
int stkMRMLIGTLServerNode::Stop()
{
  // 仅存在线程时才去停止
  if (this->ThreadID < 0)
	  return 0;

  // NOTE: Thread should be killed by activating ServerStopFlag.
  this->ServerStopFlag = true;//设置这个标准位以后，Thread里面的while循环无法进行，从而推出线程

  this->Mutex->Lock();
  if (this->Socket.IsNotNull())
  {
	  this->Socket->CloseSocket(); //关闭Socket
  }
  this->Mutex->Unlock();

  this->Thread->TerminateThread(this->ThreadID);//杀掉进程
  this->ThreadID = -1; //通过这个ID来表示没有线程的状态
  return 1;
}


//---------------------------------------------------------------------------
void* stkMRMLIGTLServerNode::ThreadFunction(void* ptr)
{
  //首先在线程里面获得stkMRMLIGTLServerNode的指针，这个指针式启动线程时通过this指针传进去的，在vinfo内部以无类型指针UserData变量保存
  vtkMultiThreader::ThreadInfo* vinfo = static_cast<vtkMultiThreader::ThreadInfo*>(ptr);
  stkMRMLIGTLServerNode* igtlcon = static_cast<stkMRMLIGTLServerNode*>(vinfo->UserData); 

  //刚开始线程的时候并未连接，是WAIT状态
  igtlcon->State = STATE_WAIT_CONNECTION;

  //创建一个igtl服务器的Socket，并通过其利用端口ServerPort启动服务；
  //如果创建不成功则将ServerStopFlag设为true，这样后继循环不执行直接跳出，进入收尾过程
  igtlcon->ServerSocket = igtl::ServerSocket::New();
  if (igtlcon->ServerSocket->CreateServer(igtlcon->ServerPort) == -1)
  {
	  vtkErrorWithObjectMacro(igtlcon, "Failed to create server socket !");
	  igtlcon->ServerStopFlag = true;
  }

  // 通信过程
  while (!igtlcon->ServerStopFlag) //每次循环需要看看这个标志位的值才决定是否继续循环
    {
    
	//函数内反复循环，等待一个Client的连接，仅有连接上以后才会结束这函数，往下执行
    igtlcon->Mutex->Lock();	
    igtlcon->WaitForConnection(); 
    igtlcon->Mutex->Unlock();

	//如果连接上了的话，这里的Socket不是Null
    if (igtlcon->Socket.IsNotNull())
      {
	  //状态设置为连接上，并发送消息，由于我们不在主线程里面，所以调用RequestInvokeEvent
      igtlcon->State = STATE_CONNECTED; 
      igtlcon->RequestInvokeEvent(stkMRMLIGTLServerNode::ConnectedEvent);

	  //vtkErrorMacro("vtkOpenIGTLinkIFLogic::ThreadFunction(): Client Connected.");
      igtlcon->ReceiveController(); //处理接收到的数据

	  //状态设置为断开连接，并发送消息，由于我们不在主线程里面，所以调用RequestInvokeEvent
      igtlcon->State = STATE_WAIT_CONNECTION;
      igtlcon->RequestInvokeEvent(stkMRMLIGTLServerNode::DisconnectedEvent);
      }
    }

  if (igtlcon->Socket.IsNotNull())
    {
    igtlcon->Socket->CloseSocket();
    }

  if (igtlcon->ServerSocket.IsNotNull())
    {
    igtlcon->ServerSocket->CloseSocket();
    }

  igtlcon->ThreadID = -1;
  igtlcon->State = STATE_OFF;
  igtlcon->RequestInvokeEvent(stkMRMLIGTLServerNode::DeactivatedEvent); // need to Request the InvokeEvent, because we are not on the main thread now

  return NULL;
}

//----------------------------------------------------------------------------
void stkMRMLIGTLServerNode::RequestInvokeEvent(unsigned long eventId)
{
  this->EventQueueMutex->Lock();
  this->EventQueue.push_back(eventId);
  this->EventQueueMutex->Unlock();
}

//----------------------------------------------------------------------------
//本函数反复执行循环等待一个连接
int stkMRMLIGTLServerNode::WaitForConnection()
{
  //如果不将ServerStopFlag设为true，这里将进入一个死循环，直至等到一个连接以后再返回1
  while (!this->ServerStopFlag) 
    {
		this->Socket = this->ServerSocket->WaitForConnection(1000);
		if (this->Socket.IsNotNull()) // if client connected
		{
			return 1;//如果等到一个连接，则返回
		}
    }

  //如果在未等到连接的情况下，再设置停止服务器，设置ServerStopFlag设为true，才能够进行到这一步
  if (this->Socket.IsNotNull())
    {
    //vtkErrorMacro("vtkOpenIGTLinkLogic::WaitForConnection(): Socket Closed.");
    this->Socket->CloseSocket();
    }

  return 0;
}


//----------------------------------------------------------------------------
int stkMRMLIGTLServerNode::ReceiveController()
{
	if (this->Socket.IsNull())
		return 0;

  //igtl_header header;
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  while (!this->ServerStopFlag)
    {
    // check if connection is alive
    if (!this->Socket->GetConnected())
      {
      break;
      }

    //----------------------------------------------------------------
    // Receive Header
    headerMsg->InitPack();

	//通过socket读取头信息，头信息里面包含了这个message各种重要信息，比如DeviceName和BodySize等
    int r = this->Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
    if (r != headerMsg->GetPackSize())
      {
      vtkErrorMacro("Irregluar size " << r << " expecting " << headerMsg->GetPackSize() );
      break;
      }

     //反序列化解析头信息
    headerMsg->Unpack();
    
    //查找CircularBuffer： 获得设备名称，并用名称作为key，找到对应的CircularBuffer，如果没有的话则新建一个
	const char* devName = headerMsg->GetDeviceName();
    std::string key = headerMsg->GetDeviceName();
    if (devName[0] == '\0') //如果字符串为空，则创造一个key
      {
      // The following device name never conflicts with any device names comming from OpenIGTLink message, since the number of characters is beyond the limit.
      std::stringstream ss;
      ss << "OpenIGTLink_MESSAGE_" << headerMsg->GetDeviceType();
      key = ss.str();
      }
	
    CircularBufferMap::iterator iter = this->Buffer.find(key);
    if (iter == this->Buffer.end()) // First time to refer the device name
      {
      this->CircularBufferMutex->Lock();
      this->Buffer[key] = stkIGTLCircularBuffer::New();
      this->CircularBufferMutex->Unlock();
      }

    //----------------------------------------------------------------
    // 将body数据读取到circularbuffer里面特定的对象里面
	// 找到特定设备对应的buffer，并将接收到的数据放到buffer里面
    stkIGTLCircularBuffer* circBuffer = this->Buffer[key];
    if (circBuffer && circBuffer->StartPush() != -1)
      {      
      circBuffer->StartPush();

      igtl::MessageBase::Pointer buffer = circBuffer->GetPushBuffer();
      buffer->SetMessageHeader(headerMsg); //在这步骤里面会获得bodysize这个重要的信息
      buffer->AllocatePack();//在这里会根据bodysize重新分配整个message的缓冲区，并将原来的头信息copy过去

      int read = this->Socket->Receive(buffer->GetPackBodyPointer(), buffer->GetPackBodySize()); //这里通过socket获取body的数据
      if (read != buffer->GetPackBodySize())
        {
        vtkErrorMacro ("Only read " << read << " but expected to read " << buffer->GetPackBodySize() << "\n");
        continue;
        }

      circBuffer->EndPush();
      }
    else
      {
      break;
      }

    } // while (!this->ServerStopFlag)，只要不停止，就一直在这个函数里面循环接收新消息

  this->Socket->CloseSocket();

  return 0;

}


//----------------------------------------------------------------------------
int stkMRMLIGTLServerNode::SendData(int size, unsigned char* data)
{
  if (this->Socket.IsNull())
    return 0;

  // check if connection is alive
  if (!this->Socket->GetConnected())
    return 0;

  return this->Socket->Send(data, size);  // return 1 on success, otherwise 0.

}


//----------------------------------------------------------------------------
int stkMRMLIGTLServerNode::Skip(int length, int skipFully)
{
  unsigned char dummy[256];
  int block  = 256;
  int n      = 0;
  int remain = length;

  do
    {
    if (remain < block)
      {
      block = remain;
      }

    n = this->Socket->Receive(dummy, block, skipFully);
    remain -= n;
    }
  while (remain > 0 || (skipFully && n < block));

  return (length - remain);
}


//----------------------------------------------------------------------------
unsigned int stkMRMLIGTLServerNode::GetUpdatedBuffersList(NameListType& nameList)
{
  nameList.clear();

  CircularBufferMap::iterator iter;
  for (iter = this->Buffer.begin(); iter != this->Buffer.end(); iter ++)
    {
    if (iter->second != NULL && iter->second->IsUpdated())
      nameList.push_back(iter->first);
    }
  return nameList.size();
}


//----------------------------------------------------------------------------
stkIGTLCircularBuffer* stkMRMLIGTLServerNode::GetCircularBuffer(std::string& key)
{
  CircularBufferMap::iterator iter = this->Buffer.find(key);
  if (iter != this->Buffer.end())
    return this->Buffer[key]; 
  else
    return NULL; 
}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::ImportDataFromCircularBuffer()
{
  stkMRMLIGTLServerNode::NameListType nameList;
  GetUpdatedBuffersList(nameList); //首先看看哪些buffer里面有新的数据,如果没有的话nameList为空

  stkMRMLIGTLServerNode::NameListType::iterator nameIter;
  for (nameIter = nameList.begin(); nameIter != nameList.end(); nameIter ++)
    {
    //按照devicename获得特定的circBuffer
    stkIGTLCircularBuffer* circBuffer = GetCircularBuffer(*nameIter);

    circBuffer->StartPull();
    igtl::MessageBase::Pointer buffer = circBuffer->GetPullBuffer();

	//根据名字找到对应的Converter
    MessageConverterMapType::iterator conIter =  this->IGTLNameToConverterMap.find(buffer->GetDeviceType());
    if (conIter == this->IGTLNameToConverterMap.end()) // couldn't find from the map
      {
      continue;
      }

    if (strncmp("OpenIGTLink_MESSAGE_", (*nameIter).c_str(), 20) == 0)
      {
      buffer->SetDeviceName("OpenIGTLink");
      }

    stkIGTLToMRMLBase* converter = conIter->second;

    vtkMRMLScene* scene = this->GetScene();

    vtkMRMLNode* updatedNode = NULL;

    int found = 0;

    // look up the incoming MRML node list
    NodeInfoListType::iterator inIter;
    for (inIter = this->IncomingMRMLNodeInfoList.begin();
         inIter != this->IncomingMRMLNodeInfoList.end();
         inIter ++)
      {
      vtkMRMLNode* node = (*inIter).node;
      if (strcmp(node->GetNodeTagName(), converter->GetMRMLName()) == 0 &&
          strcmp(node->GetName(), (*nameIter).c_str()) == 0)
        {
        if ((*inIter).lock == 0)
          {
		  //BEGIN:将IGTL消息的信息转换到MRML结点里面
          node->DisableModifiedEventOn();
          converter->IGTLToMRML(buffer, node);
          // Save OpenIGTLink time stamp
          igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
          buffer->GetTimeStamp(ts);
          (*inIter).second = ts->GetSecond();
          (*inIter).nanosecond = ts->GetNanosecond();
          //node->SetAttribute("IGTLTime", )
          node->Modified();  // in case converter doesn't call any Modified()s
          node->DisableModifiedEventOff();
          node->InvokePendingModifiedEvent();
          updatedNode = node;
		  //END:将IGTL消息的信息转换到MRML结点里面
          }
        found = 1;
        break;
        }
      }

	//如果第一次接受到消息，则没有对应的结点，就要创建一个
    if (!found)
      {
        const char* classname = scene->GetClassNameByTag(converter->GetMRMLName());
        vtkCollection* collection = scene->GetNodesByClassByName(classname, buffer->GetDeviceName());
        int nCol = collection->GetNumberOfItems();
        if (nCol == 0) //下面代码创建一个新的Device
          {
          vtkMRMLNode* node = converter->CreateNewNode(this->GetScene(), buffer->GetDeviceName());
          NodeInfoType* nodeInfo = RegisterIncomingMRMLNode(node);

		  //BEGIN:将IGTL消息的信息转换到MRML结点里面
          node->DisableModifiedEventOn();
          converter->IGTLToMRML(buffer, node);
          igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
          buffer->GetTimeStamp(ts);
          nodeInfo->second = ts->GetSecond();
          nodeInfo->nanosecond = ts->GetNanosecond();
          node->Modified();  // in case converter doesn't call any Modifieds itself
          node->DisableModifiedEventOff();
          node->InvokePendingModifiedEvent();
          updatedNode = node;
		  //END:将IGTL消息的信息转换到MRML结点里面


          this->InvokeEvent(stkMRMLIGTLServerNode::NewDeviceEvent);
          }
        else
          {
          for (int i = 0; i < nCol; i ++)
            {
            vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(collection->GetItemAsObject(i));
            NodeInfoType* nodeInfo = RegisterIncomingMRMLNode(node);
            node->DisableModifiedEventOn();
            converter->IGTLToMRML(buffer, node);

            // Save OpenIGTLink time stamp
            // TODO: avoid calling New() every time.
            igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
            buffer->GetTimeStamp(ts);
            nodeInfo->second = ts->GetSecond();
            nodeInfo->nanosecond = ts->GetNanosecond();

            node->Modified();  // in case converter doesn't call any Modifieds itself
            node->DisableModifiedEventOff();
            node->InvokePendingModifiedEvent();
            updatedNode = node;
            break;
            // TODO: QueueNode supposes that there is only unique combination of type and node name,
            // but it should be able to hold multiple nodes.
            }
          }
        collection->Delete();
      }

    this->InvokeEvent(stkMRMLIGTLServerNode::ReceiveEvent);
    circBuffer->EndPull();
    }

}

//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::ImportEventsFromEventBuffer()
{
  // Invoke all events in the EventQueue

  bool emptyQueue=true;
  unsigned long eventId=0;
  do
  {
    emptyQueue=true;
    this->EventQueueMutex->Lock();
    if (this->EventQueue.size()>0)
    {
      eventId=this->EventQueue.front();
      this->EventQueue.pop_front();
      emptyQueue=false;
    }
    this->EventQueueMutex->Unlock();

    // Invoke the event
    this->InvokeEvent(eventId);

  } while (!emptyQueue);

}


//---------------------------------------------------------------------------
int stkMRMLIGTLServerNode::RegisterMessageConverter(stkIGTLToMRMLBase* converter)
{
  if (converter == NULL)
    {
      return 0;
    }

  // Check if the same converter has already been registered.
  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    if ((converter->GetIGTLName() && strcmp(converter->GetIGTLName(), (*iter)->GetIGTLName()) == 0) &&
        (converter->GetMRMLName() && strcmp(converter->GetMRMLName(), (*iter)->GetMRMLName()) == 0))
      {
      return 0;
      }
    }

  // Register the converter
  if (converter->GetIGTLName() && converter->GetMRMLName())
    {
    // check the converter type (single IGTL name or multiple IGTL names?)
    if (converter->GetConverterType() == stkIGTLToMRMLBase::TYPE_NORMAL)
      {
      const char* name = converter->GetIGTLName();

      // Check if the name already exists.
      MessageConverterMapType::iterator citer;
      citer = this->IGTLNameToConverterMap.find(name);
      if (citer != this->IGTLNameToConverterMap.end()) // exists
        {
        std::cerr << "The converter with the same IGTL name has already been registered." << std::endl;
        return 0;
        }
      else
        {
        // Add converter to the map
        this->IGTLNameToConverterMap[name] = converter;
        }
      }

    else // stkIGTLToMRMLBase::TYPE_MULTI_IGTL_NAMES
      {
      int numNames = converter->GetNumberOfIGTLNames();

      // Check if one of the names already exists.
      for (int i = 0; i < numNames; i ++)
        {
        const char* name = converter->GetIGTLName(i);
        MessageConverterMapType::iterator citer;
        citer = this->IGTLNameToConverterMap.find(name);
        if (citer != this->IGTLNameToConverterMap.end()) // exists
          {
          std::cerr << "The converter with the same IGTL name has already been registered." << std::endl;
          return 0;
          }
        }

      for (int i = 0; i < numNames; i ++)
        {
        // Add converter to the map
        const char* name = converter->GetIGTLName(i);
        this->IGTLNameToConverterMap[name] = converter;
        }

      }

    // Set CRC check flag
    converter->SetCheckCRC(this->CheckCRC);

    // Add the converter to the list
    this->MessageConverterList.push_back(converter);

    return 1;
    }
  else
    {
    return 0;
    }
}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::UnregisterMessageConverter(stkIGTLToMRMLBase* converter)
{

  if (converter)
    {
    MessageConverterListType::iterator iter;
    for (iter = this->MessageConverterList.begin(); iter != this->MessageConverterList.end(); iter ++)
      {
      if (*iter == converter)
        {
        this->MessageConverterList.erase(iter); // TODO: Does this work properly? Does the iterator need to be incremented?
        }
      }
    }

}


//---------------------------------------------------------------------------
int stkMRMLIGTLServerNode::RegisterOutgoingMRMLNode(vtkMRMLNode* node, const char* devType)
{

  if (!node)
    {
    return 0;
    }

  // Find a converter for the node
  stkIGTLToMRMLBase* converter = NULL;
  if (devType == NULL)
    {
    const char* tag = node->GetNodeTagName();
    converter = GetConverterByMRMLTag(tag);
    }
  else
    {
    converter = GetConverterByIGTLDeviceType(devType);
    }
  if (!converter)
    {
    return 0;
    }

  // Check if the node has already been reagistered.
  MRMLNodeListType::iterator iter;
  for (iter = this->OutgoingMRMLNodeList.begin(); iter != this->OutgoingMRMLNodeList.end(); iter ++)
    {
    if (*iter == node) // the node has been already registered.
      {
      // Unregister events
      vtkIntArray* nodeEvents = converter->GetNodeEvents();
      int n = nodeEvents->GetNumberOfTuples();
      for (int i = 0; i < n; i ++)
        {
        int event = nodeEvents->GetValue(i);
        vtkEventBroker::GetInstance()
          ->RemoveObservations(*iter, event, this, this->MRMLCallbackCommand );
        }
      this->OutgoingMRMLNodeList.erase(iter);
      break;
      }
    }

  this->MRMLIDToConverterMap[node->GetID()] = converter;
  this->OutgoingMRMLNodeList.push_back(node);

  vtkIntArray* nodeEvents = converter->GetNodeEvents();
  int n = nodeEvents->GetNumberOfTuples();
  for (int i = 0; i < n; i ++)
    {
    // regiser events
    int event = nodeEvents->GetValue(i);
    vtkEventBroker::GetInstance()
      ->AddObservation(node, event, this, this->MRMLCallbackCommand );
    }

  this->Modified();

  return 1;

}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::UnregisterOutgoingMRMLNode(vtkMRMLNode* node)
{
  if (!node)
    {
    return;
    }

  // Check if the node has already been reagistered.
  MRMLNodeListType::iterator iter;
  for (iter = this->OutgoingMRMLNodeList.begin(); iter != this->OutgoingMRMLNodeList.end(); iter ++)
    {
    if (*iter == node) // the node has been already registered.
      {
      vtkEventBroker::GetInstance()
        ->RemoveObservations(*iter, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
      this->OutgoingMRMLNodeList.erase(iter);

      // Search converter from MRMLIDToConverterMap
      MessageConverterMapType::iterator citer = this->MRMLIDToConverterMap.find(node->GetID());
      if (citer != this->MRMLIDToConverterMap.end())
        {
        this->MRMLIDToConverterMap.erase(citer);
        break;
        }
      break;
      }
    }
}


//---------------------------------------------------------------------------
stkMRMLIGTLServerNode::NodeInfoType* stkMRMLIGTLServerNode::RegisterIncomingMRMLNode(vtkMRMLNode* node)
{

  if (!node)
    {
    return NULL;
    }

  // Check if the node has already been registered.
  NodeInfoListType::iterator iter;
  for (iter = this->IncomingMRMLNodeInfoList.begin(); iter != this->IncomingMRMLNodeInfoList.end(); iter ++)
    {
    if ((*iter).node == node) // the node has been already registered.
      {
      return &(*iter);
      }
    }

  // Add new NodeInfoType structure
  NodeInfoType nodeInfo;
  nodeInfo.node = node;
  nodeInfo.lock = 0;
  nodeInfo.second = 0;
  nodeInfo.nanosecond = 0;
  this->IncomingMRMLNodeInfoList.push_back(nodeInfo);
  this->Modified();

  return &(this->IncomingMRMLNodeInfoList.back());
}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::UnregisterIncomingMRMLNode(vtkMRMLNode* node)
{

  if (!node)
    {
    return;
    }

  // Check if the node has already been reagistered.
  // TODO: MRMLNodeListType can be reimplemented as a std::list
  // so that the converter can be removed by 'remove()' method.
  NodeInfoListType::iterator iter;
  for (iter = this->IncomingMRMLNodeInfoList.begin(); iter != this->IncomingMRMLNodeInfoList.end(); iter ++)
    {
    if ((*iter).node == node) // the node has been already registered.
      {
      this->IncomingMRMLNodeInfoList.erase(iter);
      break;
      }
    }

}


//---------------------------------------------------------------------------
unsigned int stkMRMLIGTLServerNode::GetNumberOfOutgoingMRMLNodes()
{
  return this->OutgoingMRMLNodeList.size();
}


//---------------------------------------------------------------------------
vtkMRMLNode* stkMRMLIGTLServerNode::GetOutgoingMRMLNode(unsigned int i)
{
  if (i < this->OutgoingMRMLNodeList.size())
    {
    return this->OutgoingMRMLNodeList[i];
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
stkIGTLToMRMLBase* stkMRMLIGTLServerNode::GetConverterByNodeID(const char* id)
{

  MessageConverterMapType::iterator iter;
  iter = this->MRMLIDToConverterMap.find(id);
  if (iter == this->MRMLIDToConverterMap.end())
    {
    return NULL;
    }
  return iter->second;

}


//---------------------------------------------------------------------------
unsigned int stkMRMLIGTLServerNode::GetNumberOfIncomingMRMLNodes()
{
  return this->IncomingMRMLNodeInfoList.size();
}


//---------------------------------------------------------------------------
vtkMRMLNode* stkMRMLIGTLServerNode::GetIncomingMRMLNode(unsigned int i)
{

  if (i < this->IncomingMRMLNodeInfoList.size())
    {
    return this->IncomingMRMLNodeInfoList[i].node;
    }
  else
    {
    return NULL;
    }

}


//---------------------------------------------------------------------------
stkIGTLToMRMLBase* stkMRMLIGTLServerNode::GetConverterByMRMLTag(const char* tag)
{

  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    if ((*iter) && strcmp((*iter)->GetMRMLName(), tag) == 0)
      {
      return *iter;
      }
    }

  // if no converter is found.
  return NULL;

}


//---------------------------------------------------------------------------
stkIGTLToMRMLBase* stkMRMLIGTLServerNode::GetConverterByIGTLDeviceType(const char* type)
{
  MessageConverterListType::iterator iter;

  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    stkIGTLToMRMLBase* converter = *iter;
    if (converter->GetConverterType() == stkIGTLToMRMLBase::TYPE_NORMAL)
      {
      if (strcmp(converter->GetIGTLName(), type) == 0)
        {
        return converter;
        }
      }
    else // The converter has multiple IGTL device names
      {
      int n = converter->GetNumberOfIGTLNames();
      for (int i = 0; i < n; i ++)
        {
        if (strcmp(converter->GetIGTLName(i), type) == 0)
          {
          return converter;
          }
        }
      }
    }

  // if no converter is found.
  return NULL;

}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::PushNode(vtkMRMLNode* node)
{
  if (!node)
    {
    return;
    }

  stkIGTLToMRMLBase* converter = this->MRMLIDToConverterMap[node->GetID()];
  if (!converter)
    {
    return;
    }

  vtkIntArray* eventlist = converter->GetNodeEvents();
  if (eventlist->GetNumberOfTuples() > 0)
    {
    node->InvokeEvent(eventlist->GetValue(0));
    }
}

//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::LockIncomingMRMLNode(vtkMRMLNode* node)
{
  NodeInfoListType::iterator iter;
  for (iter = this->IncomingMRMLNodeInfoList.begin(); iter != this->IncomingMRMLNodeInfoList.end(); iter++)
    {
    if ((*iter).node == node)
      {
      (*iter).lock = 1;
      }
    }

}


//---------------------------------------------------------------------------
void stkMRMLIGTLServerNode::UnlockIncomingMRMLNode(vtkMRMLNode* node)
{
  // Check if the node has already been added in the locked node list
  NodeInfoListType::iterator iter;
  for (iter = this->IncomingMRMLNodeInfoList.begin(); iter != this->IncomingMRMLNodeInfoList.end(); iter++)
    {
    if ((*iter).node == node)
      {
      (*iter).lock = 0;
      return;
      }
    }
}


//---------------------------------------------------------------------------
int stkMRMLIGTLServerNode::GetIGTLTimeStamp(vtkMRMLNode* node, int& second, int& nanosecond)
{
  // Check if the node has already been added in the locked node list
  NodeInfoListType::iterator iter;
  for (iter = this->IncomingMRMLNodeInfoList.begin(); iter != this->IncomingMRMLNodeInfoList.end(); iter++)
    {
    if ((*iter).node == node)
      {
      second = (*iter).second;
      nanosecond = (*iter).nanosecond;
      return 1;
      }
    }

  return 0;

}


