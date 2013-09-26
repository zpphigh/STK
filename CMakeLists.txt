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

# Append the library version information to the library target
# properties.  A parent project may set its own properties and/or may
# block this.
if(NOT STK_NO_LIBRARY_VERSION)
  set(STK_LIBRARY_PROPERTIES ${STK_LIBRARY_PROPERTIES}
    VERSION "${STK_VERSION}"
    SOVERSION "${STK_MAJOR_VERSION}.${STK_MINOR_VERSION}"
    )
endif()


#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
if(NOT STK_INSTALL_BIN_DIR)
  set(STK_INSTALL_BIN_DIR "bin")
endif()
if(NOT STK_INSTALL_LIB_DIR)
  set(STK_INSTALL_LIB_DIR "lib/stk-${STK_MAJOR_VERSION}.${STK_MINOR_VERSION}")
endif()
if(NOT STK_INSTALL_INCLUDE_DIR)
  set(STK_INSTALL_INCLUDE_DIR "include/stk-${STK_MAJOR_VERSION}.${STK_MINOR_VERSION}")
endif()
if(NOT STK_INSTALL_DOC_DIR)
  set(STK_INSTALL_DOC_DIR "doc")
endif()


#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})


#-----------------------------------------------------------------------------
# Clear STK_BASE_INCLUDE_DIRS, STK_BASE_LIBRARIES and STK_WRAPPED_LIBRARIES_PYTHONQT
#
set(STK_BASE_LIBRARIES CACHE INTERNAL "STK base libraries" FORCE)
set(STK_BASE_INCLUDE_DIRS CACHE INTERNAL "STK includes" FORCE)
set(STK_WRAPPED_LIBRARIES_PYTHONQT CACHE INTERNAL "STK libraries wrapped using PythonQt" FORCE)

# Variable use in STKConfig.cmake.in
set(STK_LIBRARIES CACHE INTERNAL "STK libraries" FORCE)
set(${PROJECT_NAME}_PLUGIN_LIBRARIES CACHE INTERNAL "STK plugins" FORCE)

# Used by STKGenerateSTKConfig.cmake and also used to reference script from other scripts
set(STK_CMAKE_DIR ${STK_SOURCE_DIR}/CMake)
set(STK_CMAKE_UTILITIES_DIR ${STK_SOURCE_DIR}/Utilities/CMake)

#-----------------------------------------------------------------------------
# CMake function(s) and macro(s)
#
include(CMake/stkMacroParseArguments.cmake)
#include(CMake/stkMacroSetPaths.cmake)
include(CMake/stkMacroListFilter.cmake)
include(CMake/stkMacroOptionUtils.cmake)
include(CMake/stkMacroBuildLib.cmake)
#include(CMake/stkMacroBuildLibWrapper.cmake)
#include(CMake/stkMacroBuildPlugin.cmake)
include(CMake/stkMacroBuildApp.cmake)
#include(CMake/stkMacroBuildQtPlugin.cmake)
#include(CMake/stkMacroCompilePythonScript.cmake)
#include(CMake/stkMacroGenerateMocs.cmake)
#include(CMake/stkMacroWrapPythonQt.cmake)
include(CMake/stkMacroSetupQt.cmake)
include(CMake/stkMacroTargetLibraries.cmake) # Import multiple macros
#include(CMake/stkFunctionExtractOptionNameAndValue.cmake)
#include(CMake/stkMacroValidateBuildOptions.cmake)
#include(CMake/stkMacroAddCtkLibraryOptions.cmake)
#include(CMake/stkFunctionGenerateDGraphInput.cmake)
#include(CMake/stkFunctionGenerateProjectXml.cmake)
#include(CMake/stkFunctionGeneratePluginManifest.cmake)
#include(CMake/stkMacroGeneratePluginResourceFile.cmake)
#include(CMake/stkFunctionAddPluginRepo.cmake)
#include(CMake/stkFunctionCheckCompilerFlags.cmake)
#include(CMake/stkFunctionCheckoutRepo.cmake)
include(CMake/stkFunctionGetIncludeDirs.cmake)
include(CMake/stkFunctionGetLibraryDirs.cmake)
#include(CMake/stkFunctionGetGccVersion.cmake)
#include(CMake/stkFunctionGetCompilerVisibilityFlags.cmake)
#include(CMake/stkFunctionCompileSnippets.cmake)


# QT
#
stkMacroSetupQt()
# Update CTK_BASE_LIBRARIES with QT libraries
if(QT4_FOUND)
  set(CTK_BASE_LIBRARIES ${CTK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "CTK base libraries" FORCE)
endif()
 

#set output path, we currently use the FITSDK environment
set(EXECUTABLE_OUTPUT_PATH $ENV{FITSDK}/bin)
set(LIBRARY_OUTPUT_PATH $ENV{FITSDK}/bin)

add_subdirectory(Libs/Hyperthermia)
add_subdirectory(Libs/RoboticSurgery)
add_subdirectory(Applications/stkRobotUR5Server)
