
#include "stkTrackerThread.h"


class stkTrackerThreadPrivate
{
public:

};


stkTrackerThread::stkTrackerThread(QObject *parent)
	: QThread(parent),d_ptr(new stkTrackerThreadPrivate)
{
	Q_D(stkTrackerThread);

}

stkTrackerThread::~stkTrackerThread()
{

}
