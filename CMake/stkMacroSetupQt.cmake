###########################################################################
#
#  Library:   STK
#
#  Copyright (c) fitark.org
#
#	National Laboratory for Information Science and Technology, Tsinghua University
#	Department of Interventional Ultrasound, Chinese PLA General Hospital
#
###########################################################################

#! \ingroup CMakeUtilities
macro(stkMacroSetupQt)

  set(minimum_required_qt_version "4.7")

  find_package(Qt4)
  
  if(QT4_FOUND)

    if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
      message(FATAL_ERROR "error: STK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    endif()

    set(QT_USE_QTNETWORK ON)
    set(QT_USE_QTSQL ON)
    set(QT_USE_QTOPENGL ON)
    set(QT_USE_QTXML ON)
    set(QT_USE_QTXMLPATTERNS ON)
    set(QT_USE_QTTEST ${BUILD_TESTING})
    include(${QT_USE_FILE})

    # Set variable QT_INSTALLED_LIBRARY_DIR that will contains
    # Qt shared library
    set(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
    if(WIN32)
      get_filename_component(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
    endif()

  else()
    message(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
  endif()

endmacro()
