/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

// STK includes
#include "stkSurgeryNavigationExport.h"

// OpenIGTLinkIF MRML includes
#include "stkIGTLToMRMLBase.h"


// OpenIGTLink includes
#include <igtlServerSocket.h>
#include <igtlClientSocket.h>

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLStorageNode.h>

// VTK includes
#include <vtkObject.h>

// STD includes
#include <string>
#include <map>
#include <vector>
#include <set>

class vtkMultiThreader;
class vtkMutexLock;
class stkIGTLCircularBuffer;

class STK_SURGERYNAVIGATION_EXPORT stkMRMLIGTLServerNode : public vtkMRMLNode
{

 public:

  // Events
  enum {
    ConnectedEvent        = 118944,
    DisconnectedEvent     = 118945,
    ActivatedEvent        = 118946,
    DeactivatedEvent      = 118947,
    ReceiveEvent          = 118948,
    NewDeviceEvent        = 118949,
    DeviceModifiedEvent   = 118950
  };

  //Connector״̬����
  enum {
    STATE_OFF,
    STATE_WAIT_CONNECTION,
    STATE_CONNECTED,
    NUM_STATE
  };
  static const char* ConnectorStateStr[stkMRMLIGTLServerNode::NUM_STATE];

  //
  enum {
    IO_UNSPECIFIED = 0x00,
    IO_INCOMING   = 0x01,
    IO_OUTGOING   = 0x02,
  };

  //�豸�б�ͨ��DeviceID������
  typedef struct {
	  std::string   name;
	  std::string   type;
	  int           io;
  } DeviceInfoType;

  typedef std::set<int>                   DeviceIDSetType;	//�����豸ID�ļ���
  typedef std::map<int, DeviceInfoType>   DeviceInfoMapType;   // Device list:  index is referred as a device id in the connector.
  
  typedef struct {
	  vtkMRMLNode*  node;
	  int           lock;
	  int           second;
	  int           nanosecond;
  } NodeInfoType;
  typedef std::vector<NodeInfoType>       NodeInfoListType;


  typedef std::vector<vtkMRMLNode*>       MRMLNodeListType;

  typedef std::list<stkIGTLToMRMLBase*>   MessageConverterListType;
  typedef std::map<std::string, stkIGTLToMRMLBase*> MessageConverterMapType;

 public:

  //----------------------------------------------------------------
  // �̳�MRML Nodes����Ҫʵ�ֵı�׼����
  //----------------------------------------------------------------
  static stkMRMLIGTLServerNode *New();
  vtkTypeMacro(stkMRMLIGTLServerNode,vtkMRMLNode);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "IGTLConnector";};

  // ��������̳���vtkMRMLNode: method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------

  stkMRMLIGTLServerNode();
  ~stkMRMLIGTLServerNode();
  stkMRMLIGTLServerNode(const stkMRMLIGTLServerNode&);
  void operator=(const stkMRMLIGTLServerNode&);


 public:
  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------

  vtkGetMacro( ServerPort, int );
  vtkSetMacro( ServerPort, int );
  vtkGetMacro( State, int );
  vtkGetMacro( CheckCRC, int );
  void SetCheckCRC(int c);

  //----------------------------------------------------------------
  // �߳̿���
  //----------------------------------------------------------------
  int Start();
  int Stop();
  static void* ThreadFunction(void* ptr);

  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  int WaitForConnection();
  int ReceiveController();
  int SendData(int size, unsigned char* data);
  int Skip(int length, int skipFully=1);

  //----------------------------------------------------------------
  // Circular Buffer
  //----------------------------------------------------------------

  typedef std::vector<std::string> NameListType;
  unsigned int GetUpdatedBuffersList(NameListType& nameList); // TODO: this will be moved to private
  stkIGTLCircularBuffer* GetCircularBuffer(std::string& key);     // TODO: Is it OK to use device name as a key?

  //----------------------------------------------------------------
  // Device Lists
  //----------------------------------------------------------------

  // Description:
  // Import received data from the circular buffer to the MRML scne.
  // This is currently called by vtkOpenIGTLinkIFLogic class.
  // ����IGTL��Ϣ��CircularBuffer��������յ���IGTL����Ϣ����������IGTL����Ϣת����MRML�����ж�Ӧ�Ľڵ��У�û�еĻ�����һ����
  void ImportDataFromCircularBuffer();

  // Description:
  // Import events from the event buffer to the MRML scene.
  // This is currently called by vtkOpenIGTLinkIFLogic class.
  // �����߳��ж��������Event������ֻ��������stkMRMLIGTLServerNode::ConnectedEvent��stkMRMLIGTLServerNode::DisconnectedEvent��stkMRMLIGTLServerNode::DeactivatedEvent
  // ��3��Event��RequestInvokeEvent��������ӵ�EventQueue���棬Ȼ�������߳��б�����Ҳ����ͨ�����̵߳�this->InvokeEvent(eventId)��������Ϣ��Invoke��������õ�����vtkObject����Ϣ����
  void ImportEventsFromEventBuffer();

  // Description:
  // Register IGTL to MRML message converter.
  // This is used by vtkOpenIGTLinkIFLogic class.
  int RegisterMessageConverter(stkIGTLToMRMLBase* converter);

  // Description:
  // Unregister IGTL to MRML message converter.
  // This is used by vtkOpenIGTLinkIFLogic class.
  void UnregisterMessageConverter(stkIGTLToMRMLBase* converter);

  // Description:
  // Set and start observing MRML node for outgoing data.
  // If devType == NULL, a converter is chosen based only on MRML Tag.
  int RegisterOutgoingMRMLNode(vtkMRMLNode* node, const char* devType=NULL);

  // Description:
  // Stop observing and remove MRML node for outgoing data.
  void UnregisterOutgoingMRMLNode(vtkMRMLNode* node);

  // Description:
  // Register MRML node for incoming data.
  // Returns a pointer to the node information in IncomingMRMLNodeInfoList
  NodeInfoType* RegisterIncomingMRMLNode(vtkMRMLNode* node);

  // Description:
  // Unregister MRML node for incoming data.
  void UnregisterIncomingMRMLNode(vtkMRMLNode* node);

  // Description:
  // Get number of registered outgoing MRML nodes:
  unsigned int GetNumberOfOutgoingMRMLNodes();

  // Description:
  // Get Nth outgoing MRML nodes:
  vtkMRMLNode* GetOutgoingMRMLNode(unsigned int i);

  // Description:
  // Get MRML to IGTL converter assigned to the specified MRML node ID
  stkIGTLToMRMLBase* GetConverterByNodeID(const char* id);

  // Description:
  // Get number of registered outgoing MRML nodes:
  unsigned int GetNumberOfIncomingMRMLNodes();

  // Description:
  // Get Nth outgoing MRML nodes:
  vtkMRMLNode* GetIncomingMRMLNode(unsigned int i);

  // Description:
  // A function to explicitly push node to OpenIGTLink
  // (Usually, data stored in MRML scene are exported, when the registered events are invoked.)
  void PushNode(vtkMRMLNode* node);


  //----------------------------------------------------------------
  // For OpenIGTLink time stamp access
  //----------------------------------------------------------------

  // Description:
  // Turn lock flag on to stop updating MRML node. Call this function before
  // reading the content of the MRML node and the corresponding time stamp.
  void LockIncomingMRMLNode(vtkMRMLNode* node);

  // Description:
  // Turn lock flag off to start updating MRML node. Make sure to call this function
  // after reading the content / time stamp.
  void UnlockIncomingMRMLNode(vtkMRMLNode* node);

  // Description:
  // Get OpenIGTLink's time stamp information. Returns 0, if it fails to obtain time stamp.
  int GetIGTLTimeStamp(vtkMRMLNode* node, int& second, int& nanosecond);


 private:

  stkIGTLToMRMLBase* GetConverterByMRMLTag(const char* tag);
  stkIGTLToMRMLBase* GetConverterByIGTLDeviceType(const char* type);

  // Description:
  // Inserts the eventId to the EventQueue, and the event will be invoked from the main thread
  void RequestInvokeEvent(unsigned long eventId);

 private:
  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------
  std::string Name;
  int State;

  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------

  vtkMultiThreader* Thread;
  vtkMutexLock*     Mutex;
  igtl::ServerSocket::Pointer  ServerSocket;
  igtl::ClientSocket::Pointer  Socket;
  int               ThreadID;
  int               ServerPort;
  int               ServerStopFlag; //�������־�����Ʒ������Ƿ�while����


  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------


  typedef std::map<std::string, stkIGTLCircularBuffer*> CircularBufferMap;
  CircularBufferMap Buffer; //

  vtkMutexLock* CircularBufferMutex;

  // Event queueing mechanism is needed to send all event notifications from the main thread.
  // Events can be pushed to the end of the EventQueue by calling RequestInvoke from any thread,
  // and they will be Invoked in the main thread.
  // �߳��в������¼�����Ҫ�����߳��д����ʱ����������EventQueue�Ļ��ƣ�
  // ������˵������ֻ��������stkMRMLIGTLServerNode::ConnectedEvent��stkMRMLIGTLServerNode::DisconnectedEvent��stkMRMLIGTLServerNode::DeactivatedEvent
  // ��Щevent������stkMRMLIGTLServerNode::ThreadFunction�������ɵģ������Ժ�ֱ�ӱ��ŵ�EventQueue����
  // Ȼ�������߳��л᲻�ϵ���ImportEventsFromEventBuffer������������������Ϣ������һ��ת����Ҳ���������߳���ͨ��
  //  this->InvokeEvent(eventId)��������Ϣ��Invoke��������õ�����vtkObject����Ϣ����
  std::list<unsigned long> EventQueue;
  vtkMutexLock* EventQueueMutex;

  // -- Device Name (same as MRML node) and data type (data type string defined in OpenIGTLink)
  DeviceIDSetType   IncomingDeviceIDSet;
  DeviceIDSetType   OutgoingDeviceIDSet;
  DeviceIDSetType   UnspecifiedDeviceIDSet;

  // Message converter (IGTLToMRML)
  MessageConverterListType MessageConverterList;
  MessageConverterMapType  IGTLNameToConverterMap;
  MessageConverterMapType  MRMLIDToConverterMap;

  // List of nodes that this connector node is observing.
  MRMLNodeListType         OutgoingMRMLNodeList;
  NodeInfoListType         IncomingMRMLNodeInfoList;

  int CheckCRC;

};

