/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

  	National Laboratory for Information Science and Technology, Tsinghua University
  	Department of Interventional Ultrasound, Chinese PLA General Hospital


=========================================================================*/

#include "stkImageHelper.h"
#include <itkImage.h>

class stkImageHelperPrivate
{
public:
};


stkImageHelper::stkImageHelper(QObject *parent)
	: QObject(parent),d_ptr(new stkImageHelperPrivate)
{
	Q_D(stkImageHelper);
}

stkImageHelper::~stkImageHelper()
{

}

