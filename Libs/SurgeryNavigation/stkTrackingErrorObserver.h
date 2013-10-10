/*=========================================================================

  Library:   STK

  Copyright (c) fitark.org

=========================================================================*/
#pragma once

#include "itkCommand.h"
#include "igstkMacros.h"

class stkTrackingErrorObserver : public itk::Command
{
    public:
      typedef stkTrackingErrorObserver            Self;
      typedef ::itk::Command                   Superclass;
      typedef ::itk::SmartPointer<Self>        Pointer;
      typedef ::itk::SmartPointer<const Self>  ConstPointer;

      igstkNewMacro(Self)
      igstkTypeMacro(IGSTKErrorObserver, itk::Command)

      /*
       * When an error occurs in the tracker or serial communication component 
       * it will invoke this method with the appropriate error event object as 
       * a parameter. */
      void Execute(itk::Object *caller, const itk::EventObject & event);

      /*
       * When an error occurs in the tracker or serial communication component 
       * it will invoke this method with the appropriate error event object as 
       * a parameter. */
      void Execute(const itk::Object *caller, const itk::EventObject & event);

      /**
       * Clear the current error. */
      void ClearError();

      /**
      * If an error occured in the tracker or serial communication this method
      * will return true. */
      bool Error();

    protected:

      /**
       * Construct an error observer for all the possible errors that occur in
       * the tracker and serial communication classes. */
      stkTrackingErrorObserver();

      virtual ~stkTrackingErrorObserver(){}

    private:

      /**
       *  member variables
       */
      bool                                m_ErrorOccured;
      std::map<std::string,std::string>   m_ErrorEvent2ErrorMessage;

      //purposely not implemented
      stkTrackingErrorObserver(const Self&);
      void operator=(const Self&);
};

