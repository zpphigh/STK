###########################################################################
#
#  Library:   STK
#
#  Copyright (c) fitark.org
#
#
###########################################################################

cmake_minimum_required (VERSION 2.8.4)

#-----------------------------------------------------------------------------
project(STK)
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
# Library mode: SHARED (default) or STATIC
#
set(STK_LIBRARY_MODE "SHARED")

option(STK_BUILD_SHARED_LIBS "Build STK libraries as shared module." ON)
mark_as_advanced(STK_BUILD_SHARED_LIBS)
if(NOT STK_BUILD_SHARED_LIBS)
  set(STK_LIBRARY_MODE "STATIC")
endif()


#-----------------------------------------------------------------------------
# Set a default build type if none was specified
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()


#-----------------------------------------------------------------------------
# STK version number.  An even minor number corresponds to releases.
#
set(STK_MAJOR_VERSION 0)
set(STK_MINOR_VERSION 1)
set(STK_PATCH_VERSION 0)
set(STK_VERSION
    "${STK_MAJOR_VERSION}.${STK_MINOR_VERSION}.${STK_PATCH_VERSION}")

#Find Qt4
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(QT_USE_QTMAIN TRUE) 

#set output path, we currently use the FITSDK environment
set(EXECUTABLE_OUTPUT_PATH $ENV{FITSDK}/bin)
set(LIBRARY_OUTPUT_PATH $ENV{FITSDK}/bin)

add_subdirectory(Libs/Hyperthermia)
add_subdirectory(Applications/stkHyperthermiaApp)
add_subdirectory(Testing/stkHyperthermiaTest)
