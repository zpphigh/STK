
#include "stkTrackerTool.h"

//typedef igstk::Transform            TransformType;
//typedef ::itk::Vector<double, 3>    VectorType;
//typedef ::itk::Versor<double>       VersorType;

class stkTrackerToolPrivate
{
public:
	QString Name;
	bool DataValid;
};

stkTrackerTool::stkTrackerTool(QObject *parent)
	: QObject(parent),d_ptr(new stkTrackerToolPrivate)
{
	Q_D(stkTrackerTool);
	d->DataValid = false;
}

stkTrackerTool::~stkTrackerTool()
{

}


QString stkTrackerTool::name() const
{
	return d_ptr->Name;
}

void stkTrackerTool::setName(QString n)
{
	Q_D(stkTrackerTool);
	d->Name = n;
}

bool stkTrackerTool::dataValid() const
{
	return d_ptr->DataValid;
}

void stkTrackerTool::setDataValid(bool v)
{
	d_ptr->DataValid = v;
	emit dataValidChanged(v);
}
