
#include "stkIGTLServerThread.h"

class stkIGTLServerThreadPrivate
{
public:
	bool AbortThread;
};


stkIGTLServerThread::stkIGTLServerThread(QObject *parent)
	: QThread(parent),d_ptr(new stkIGTLServerThreadPrivate)
{
	Q_D(stkIGTLServerThread);

}

stkIGTLServerThread::~stkIGTLServerThread()
{
	Q_D(stkIGTLServerThread);

}

void stkIGTLServerThread::run()
{
	Q_D(stkIGTLServerThread);
	d->AbortThread = false;

	while(!d->AbortThread){

		QThread::msleep(30);
	}
}

void stkIGTLServerThread::AbortThread()
{
	Q_D(stkIGTLServerThread);
	d->AbortThread = true;
}