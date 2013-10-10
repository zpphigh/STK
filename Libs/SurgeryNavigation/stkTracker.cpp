
#include "stkTracker.h"
#include "igtlClientSocket.h"

#include <ctime>
#include "igtlPositionMessage.h"

class stkTrackerPrivate
{
public:
	bool IsTracking;
	bool IsOpenned;

	QString ServerHostname;
	int ServerPort;
	igtl::ClientSocket::Pointer	clientSocket;
};


stkTracker::stkTracker(QObject *parent)
	: QObject(parent),d_ptr(new stkTrackerPrivate)
{
	Q_D(stkTracker);
	d->IsTracking = false;
	d->IsOpenned = false;
	d->clientSocket = igtl::ClientSocket::New();	
}

stkTracker::~stkTracker()
{

}

bool stkTracker::isTracking() const
{
	return d_ptr->IsTracking;
}


bool stkTracker::isOpenned() const
{
	return d_ptr->IsOpenned;
}

bool stkTracker::isServerConnected() const
{
	return  d_ptr->clientSocket->GetConnected();
}


void stkTracker::setIsTracking(bool s)
{
	Q_D(stkTracker);
	d->IsTracking = s;
}

void stkTracker::setIsOpenned(bool o)
{
	Q_D(stkTracker);
	d->IsOpenned = o;
}

void stkTracker::DelayNSeconds(int seconds)
{
	time_t start_time, cur_time;
	time(&start_time);
	do
	{
		time(&cur_time);
	}
	while((cur_time - start_time) < seconds);
}


bool stkTracker::ConnectServer(QString hostname, int port)
{
	Q_D(stkTracker);

	if( d->ServerHostname == hostname && d->ServerPort == port && isServerConnected())
		return true; //已经建立了这个连接，可直接返回

	DisconnectServer();

	d->ServerHostname = hostname;
	d->ServerPort = port;

	if( d->clientSocket->ConnectToServer(hostname.toAscii().data(),port) != 0)
		return false;

	return isServerConnected();
}

void stkTracker::DisconnectServer()
{
	Q_D(stkTracker);
	if( !isServerConnected())
		return;

	if(d->clientSocket->GetConnected())
		d->clientSocket->CloseSocket();
}


int stkTracker::SendData(const void* data, int length)
{
	Q_D(stkTracker);
	if( !isServerConnected())
		return 0;

	return d->clientSocket->Send(data,length);
}