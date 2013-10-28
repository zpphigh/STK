// OpenIGTLink includes
#include <igtlMessageBase.h>

// OpenIGTLinkIF MRML includes
#include "stkIGTLToMRMLBase.h"

// VTK includes
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro(stkIGTLToMRMLBase);
vtkCxxRevisionMacro(stkIGTLToMRMLBase, "$Revision: 10576 $");

//---------------------------------------------------------------------------
class stkIGTLToMRMLBasePrivate
{
public:
  stkIGTLToMRMLBasePrivate();
  ~stkIGTLToMRMLBasePrivate();

protected:
};

stkIGTLToMRMLBasePrivate::stkIGTLToMRMLBasePrivate()
{
}

stkIGTLToMRMLBasePrivate::~stkIGTLToMRMLBasePrivate()
{
}



//---------------------------------------------------------------------------
stkIGTLToMRMLBase::stkIGTLToMRMLBase()
{
  this->CheckCRC = 1;
  this->Private = new stkIGTLToMRMLBasePrivate;
}

//---------------------------------------------------------------------------
stkIGTLToMRMLBase::~stkIGTLToMRMLBase()
{
  if (this->Private)
    {
    delete this->Private;
    }
}

//---------------------------------------------------------------------------
void stkIGTLToMRMLBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


