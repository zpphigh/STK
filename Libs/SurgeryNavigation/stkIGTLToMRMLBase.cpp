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

  void SetSurgeryNavigationLogic(vtkSlicerSurgeryNavigationLogic* logic);
  vtkSlicerSurgeryNavigationLogic* GetSurgeryNavigationLogic();

protected:
  vtkSlicerSurgeryNavigationLogic* SurgeryNavigationLogic;
};

stkIGTLToMRMLBasePrivate::stkIGTLToMRMLBasePrivate()
{
  this->SurgeryNavigationLogic = NULL;
}

stkIGTLToMRMLBasePrivate::~stkIGTLToMRMLBasePrivate()
{
}

void stkIGTLToMRMLBasePrivate::SetSurgeryNavigationLogic(vtkSlicerSurgeryNavigationLogic* logic)
{
  this->SurgeryNavigationLogic = logic;
}


vtkSlicerSurgeryNavigationLogic* stkIGTLToMRMLBasePrivate::GetSurgeryNavigationLogic()
{
  return this->SurgeryNavigationLogic;
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


//---------------------------------------------------------------------------
void stkIGTLToMRMLBase::SetSurgeryNavigationLogic(vtkSlicerSurgeryNavigationLogic* logic)
{
  if (this->Private)
    {
    this->Private->SetSurgeryNavigationLogic(logic);
    }
}


//---------------------------------------------------------------------------
vtkSlicerSurgeryNavigationLogic* stkIGTLToMRMLBase::GetSurgeryNavigationLogic()
{
  if (this->Private)
    {
    return this->Private->GetSurgeryNavigationLogic();
    }
  else
    {
    return NULL;
    }
}


