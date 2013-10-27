// VTK includes
#include <vtkObjectFactory.h>
#include <vtkMutexLock.h>
#include "stkIGTLCircularBuffer.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// OpenIGTLink includes
#include <igtlMessageBase.h>

// STD includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro(stkIGTLCircularBuffer);
vtkCxxRevisionMacro(stkIGTLCircularBuffer, "$Revision: 10236 $");

//---------------------------------------------------------------------------
stkIGTLCircularBuffer::stkIGTLCircularBuffer()
{
  this->Mutex = vtkMutexLock::New();
  this->Mutex->Lock();
  // Allocate Circular buffer for the new device
  this->InUse = -1;
  this->Last  = -1;
  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i ++)
    {
    this->DeviceType[i] = "";
    this->Size[i]       = 0;
    this->Data[i]       = NULL;
    this->Messages[i] = igtl::MessageBase::New();
    this->Messages[i]->InitPack();
    }

  this->UpdateFlag = 0;
  this->Mutex->Unlock();
}


//---------------------------------------------------------------------------
stkIGTLCircularBuffer::~stkIGTLCircularBuffer()
{
  this->Mutex->Lock();
  this->InUse = -1;
  this->Last  = -1;
  this->Mutex->Unlock();

  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i ++)
    {
    if (this->Data[i] != NULL)
      {
      delete this->Data[i];
      }
    }
  this->Mutex->Delete();
}


//---------------------------------------------------------------------------
void stkIGTLCircularBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
// Functions to push data into the circular buffer (for receiving thread)
//
//   StartPush() :     Prepare to push data
//   GetPushBuffer():  Get MessageBase buffer from the circular buffer
//   EndPush() :       Finish pushing data. The data becomes ready to be read by monitor thread.
//
// 下面是代码具体使用的情况 （vtkFitMRMLIGTLServerNode::ReceiveController()）
//
//	std::cerr << "Pushing into the circular buffer." << std::endl;
//	circBuffer->StartPush();
//
//	igtl::MessageBase::Pointer buffer = circBuffer->GetPushBuffer();
//	buffer->SetMessageHeader(headerMsg);
//	buffer->AllocatePack();
//
//	int read = this->Socket->Receive(buffer->GetPackBodyPointer(), buffer->GetPackBodySize());
//	if (read != buffer->GetPackBodySize())
//	{
//		vtkErrorMacro ("Only read " << read << " but expected to read "
//			<< buffer->GetPackBodySize() << "\n");
//		continue;
//	}
//
//	circBuffer->EndPush();
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int stkIGTLCircularBuffer::StartPush()
{
  this->Mutex->Lock();
  this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE;
  if (this->InPush == this->InUse)
    {
    this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE; //这个跟上面那行貌似没区别啊
    }
  this->Mutex->Unlock();

  return this->InPush;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer stkIGTLCircularBuffer::GetPushBuffer()
{
  return this->Messages[this->InPush];
}

//---------------------------------------------------------------------------
void stkIGTLCircularBuffer::EndPush()
{
  this->Mutex->Lock();
  this->Last = this->InPush;
  this->UpdateFlag = 1;
  this->Mutex->Unlock();
}


//---------------------------------------------------------------------------
// Functions to pull data into the circular buffer (for monitor thread)
//
//   StartPull() :     Prepare to pull data
//   GetPullBuffer():  Get MessageBase buffer from the circular buffer
//   EndPull() :       Finish pulling data
//
//	具体的使用代码参看：vtkFitMRMLIGTLServerNode::ImportDataFromCircularBuffer()
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int stkIGTLCircularBuffer::StartPull()
{
  this->Mutex->Lock();
  this->InUse = this->Last;
  this->UpdateFlag = 0;
  this->Mutex->Unlock();
  return this->Last;   // return -1 if it is not available
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer stkIGTLCircularBuffer::GetPullBuffer()
{
  return this->Messages[this->InUse];
}


//---------------------------------------------------------------------------
void stkIGTLCircularBuffer::EndPull()
{
  this->Mutex->Lock();
  this->InUse = -1;
  this->Mutex->Unlock();
}
