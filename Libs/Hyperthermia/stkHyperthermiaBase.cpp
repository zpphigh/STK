#include <QDir>
#include <QDateTime>
#include "stkSurgeryRoute.h"
#include "stkHyperFieldRange.h"
#include "stkHyperthermiaBase_Private.h"

stkHyperthermiaBase::stkHyperthermiaBase(QWidget *parent)
	: QWidget(parent), d_ptr(new stkHyperthermiaBasePrivate)
{
	Q_D(stkHyperthermiaBase);
	d->setupUi(this);
	d->clear();
	d->computeProgressBar->setRange(0,100);
	d->computeProgressBar->setValue(0);
}

stkHyperthermiaBase::~stkHyperthermiaBase()
{
	this->clearAllParameters();
}

double* stkHyperthermiaBase::hyperOrigin() const
{
	return d_ptr->Origin;
}

void stkHyperthermiaBase::setHyperOrigin(double* v)
{
	Q_D(stkHyperthermiaBase);
	d->Origin[0] = v[0];
	d->Origin[1] = v[1];
	d->Origin[2] = v[2];
}

void stkHyperthermiaBase::setHyperOrigin(double x, double y, double z)
{
	Q_D(stkHyperthermiaBase);
	d->Origin[0] = x;
	d->Origin[1] = y;
	d->Origin[2] = z;
}

double* stkHyperthermiaBase::hyperSpacing() const
{
	return d_ptr->Spacing;
}

void stkHyperthermiaBase::setHyperSpacing(double* v)
{
	Q_D(stkHyperthermiaBase);
	d->Spacing[0] = v[0];
	d->Spacing[1] = v[1];
	d->Spacing[2] = v[2];
}

void stkHyperthermiaBase::setHyperSpacing(double x, double y, double z)
{
	Q_D(stkHyperthermiaBase);
	d->Spacing[0] = x;
	d->Spacing[1] = y;
	d->Spacing[2] = z;
}

int* stkHyperthermiaBase::hyperSize() const
{
	return d_ptr->Size;
}

void stkHyperthermiaBase::setHyperSize(int* v)
{
	Q_D(stkHyperthermiaBase);
	d->Size[0] = v[0];
	d->Size[1] = v[1];
	d->Size[2] = v[2];
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

void stkHyperthermiaBase::setHyperSize(int x, int y, int z)
{
	Q_D(stkHyperthermiaBase);
	d->Size[0] = x;
	d->Size[1] = y;
	d->Size[2] = z;
	d->FieldSize = d->Size[0] * d->Size[1] * d->Size[2];
}

int stkHyperthermiaBase::fieldSize() const
{
	return d_ptr->FieldSize;
}

void stkHyperthermiaBase::setFieldSize(int v)
{
	Q_D(stkHyperthermiaBase);
	d->FieldSize = v;
}

int stkHyperthermiaBase::totalTime() const
{
	return d_ptr->TotalTime;
}

void stkHyperthermiaBase::setTotalTime(int v)
{
	Q_D(stkHyperthermiaBase);
	d->TotalTime = v;
}

double stkHyperthermiaBase::timeInterval() const
{
	return d_ptr->TimeInterval;
}

void stkHyperthermiaBase::setTimeInterval(double v)
{
	Q_D(stkHyperthermiaBase);
	d->TimeInterval = v;
}

int stkHyperthermiaBase::fieldTime() const
{
	return d_ptr->FieldTime;
}

void stkHyperthermiaBase::setFieldTime(int v)
{
	Q_D(stkHyperthermiaBase);
	d->FieldTime = v;
}

int* stkHyperthermiaBase::maxPoint() const
{
	return d_ptr->MaxPoint;
}

void stkHyperthermiaBase::setMaxPoint(int* v)
{
	Q_D(stkHyperthermiaBase);
	d->MaxPoint[0] = v[0];
	d->MaxPoint[1] = v[1];
	d->MaxPoint[2] = v[2];
}

void stkHyperthermiaBase::setMaxPoint(int x, int y, int z)
{
	Q_D(stkHyperthermiaBase);
	d->MaxPoint[0] = x;
	d->MaxPoint[1] = y;
	d->MaxPoint[2] = z;
}

int stkHyperthermiaBase::timeSteps() const
{
	return d_ptr->TimeSteps;
}

void stkHyperthermiaBase::setTimeSteps(int v)
{
	Q_D(stkHyperthermiaBase);
	d->TimeSteps = v;
}

int stkHyperthermiaBase::computeProgress() const
{
	return d_ptr->ComputeProgress;
}

void stkHyperthermiaBase::setComputeProgress(int v)
{
	Q_D(stkHyperthermiaBase);
	int maxV = d->computeProgressBar->maximum();
	d->ComputeProgress = v > maxV ? maxV : v;
	d->computeProgressBar->setValue(d->ComputeProgress);
}

void stkHyperthermiaBase::setComputeProgressRange(int v1, int v2)
{
	Q_D(stkHyperthermiaBase);
	d->computeProgressBar->setRange(v1,v2);
}

void stkHyperthermiaBase::addRoute(stkSurgeryRoute* pRoute)
{
	Q_D(stkHyperthermiaBase);
	d->HyperRoutes.push_back(pRoute);
}

int stkHyperthermiaBase::getNumberOfRoutes() const
{
	return d_ptr->HyperRoutes.size();
}

int stkHyperthermiaBase::getNumberOfRoutes(int time ,QList<stkSurgeryRoute*> &routes) const
{
	int count = 0;
	for( int i = 0 ; i < routes.size(); i++ )
	{
		if( time >= routes[i]->startTime() && time <= routes[i]->endTime())
			count++;
	}

	return count;
}

stkSurgeryRoute* stkHyperthermiaBase::getRoute(int i)
{
	Q_D(stkHyperthermiaBase);
	if( i >= d->HyperRoutes.size() || i < 0 )
		return NULL;

	return d->HyperRoutes[i];
}

stkSurgeryRoute* stkHyperthermiaBase::getRoute(int time, int index, QList<stkSurgeryRoute*> &routes)
{
	Q_D(stkHyperthermiaBase);
	if( index < 0 || index >= getNumberOfRoutes(time,routes) )
		return NULL;

	int count = -1;
	for( int i = 0 ; i < routes.size(); i++ )
	{
		if( time >= routes[i]->startTime() && time <= routes[i]->endTime())
		{
			count++;
			if( count == index )
				return routes[i];
		}
	}

	return NULL;
}

bool stkHyperthermiaBase::computeMultiRange()
{
	Q_D(stkHyperthermiaBase);
	d->Ranges.clear();

	for( int i = 0 ; i < getNumberOfRoutes(); i++ )
	{
		stkHyperFieldRange* tempRange = new stkHyperFieldRange;	
		tempRange->clearRoutes();
		d->computeSingleRange(i, tempRange);
		tempRange->addRoute(d->HyperRoutes[i]);

		d->Ranges.push_back(tempRange);
	}
	return true;
}

QList<stkHyperFieldRange*> stkHyperthermiaBase::getRanges() const
{
	return d_ptr->Ranges;
}

stkHyperFieldRange* stkHyperthermiaBase::getRange(int i)
{
	Q_D(stkHyperthermiaBase);
	if (i>=d->Ranges.size() || i<0)
		return NULL;

	return d->Ranges[i];
}

void stkHyperthermiaBase::addRange(stkHyperFieldRange* range)
{
	Q_D(stkHyperthermiaBase);
	d->Ranges.append(range);
}

void stkHyperthermiaBase::clearRanges()
{
	Q_D(stkHyperthermiaBase);
	d->Ranges.clear();
}

void stkHyperthermiaBase::clearAllParameters()
{
	Q_D(stkHyperthermiaBase);
	d->HyperRoutes.clear();
	this->clearRanges();
	d->clear();
}

void stkHyperthermiaBase::writeOutputFiles(int* fieldSize, double* pOrigin, float* outputField, int time,  QString fieldType)
{
	QString resultPath = QString("./ThtkResults");
	QDir dir;
	if (!dir.exists(resultPath))
		dir.mkpath(resultPath);

	QString currentDT = QDateTime::currentDateTime().toString(Qt::ISODate).replace("-","").replace(":","");
	QString outputfilename = resultPath + QString("/") + fieldType + QString("output") + currentDT + QString("_") + QString::number(time) + QString(".txt");
	FILE *fr;
	fr=fopen(outputfilename.toLocal8Bit().data(),"w");

	for (int i=0; i<3; i++)
		fprintf(fr,"%d\n", fieldSize[i]);

	for (int i=0; i<3; i++)
		fprintf(fr,"%4f\n", pOrigin[i]);

	int pSize = fieldSize[0] * fieldSize[1] * fieldSize[2];

	for (int i=0; i<pSize; i++)
	{
		if ( fr != NULL)
		{
			//fprintf(fr,"%4d : %4f \n",  i, temperature);
			fprintf(fr,"%4f\n", outputField[i]);
		}
	}
	fclose(fr);
}

void stkHyperthermiaBase::writeOutputTransientFiles(int fieldSize, float* outputField, int time,  QString fieldType)
{
	QString resultPath = QString("./ThtkResults");
	QDir dir;
	if (!dir.exists(resultPath))
		dir.mkpath(resultPath);

	QString currentDT = QDateTime::currentDateTime().toString(Qt::ISODate).replace("-","").replace(":","");
	QString outputfilename = resultPath + QString("/") + fieldType + QString("output") + currentDT + QString("_") + QString::number(time) + QString(".txt");
	FILE *fr;
	fr=fopen(outputfilename.toLocal8Bit().data(),"w");

	for (int i=0; i<fieldSize; i++)
	{
		if ( fr != NULL)
		{
			//fprintf(fr,"%4d : %4f \n",  i, temperature);
			fprintf(fr,"%4f\n", outputField[i]);
		}
	}
	fclose(fr);
}