#include "stkSlicerExt.h"

class stkSlicerExtPrivate
{
public:

};

stkSlicerExt::stkSlicerExt(QObject *parent)
: QObject(parent),d_ptr(new stkSlicerExtPrivate)
{
	Q_D(stkSlicerExt);

}

stkSlicerExt::~stkSlicerExt()
{
	Q_D(stkSlicerExt);
}
