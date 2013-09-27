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
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

set(project_policies
  CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
  CMP0002 # NEW: Logical target names must be globally unique.
  CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
  CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
  CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
  CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
  CMP0007 # NEW: List command no longer ignores empty elements.
  CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
  CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
  CMP0010 # NEW: Bad variable reference syntax is an error.
  CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
  CMP0012 # NEW: if() recognizes numbers and boolean constants.
  CMP0013 # NEW: Duplicate binary directories are not allowed.
  CMP0014 # NEW: Input directories must have CMakeLists.txt
  )
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()


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
# Superbuild Option - Enabled by default
#
option(STK_SUPERBUILD "Build STK and the projects it depends on via SuperBuild.cmake." OFF)
mark_as_advanced(STK_SUPERBUILD)


#-----------------------------------------------------------------------------
# Output directories.
#
foreach(type LIBRARY RUNTIME ARCHIVE)
  # Make sure the directory exists
  if(DEFINED STK_CMAKE_${type}_OUTPUT_DIRECTORY
     AND NOT EXISTS ${STK_CMAKE_${type}_OUTPUT_DIRECTORY})
    message(FATAL_ERROR "STK_CMAKE_${type}_OUTPUT_DIRECTORY is set to a non-existing directory [${STK_CMAKE_${type}_OUTPUT_DIRECTORY}]")
  endif()

  if(STK_SUPERBUILD)
    set(output_dir ${STK_BINARY_DIR}/bin)
    if(NOT DEFINED STK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(STK_CMAKE_${type}_OUTPUT_DIRECTORY ${STK_BINARY_DIR}/STK-build/bin)
    endif()
  else()
    if(NOT DEFINED STK_CMAKE_${type}_OUTPUT_DIRECTORY)
      set(output_dir ${STK_BINARY_DIR}/bin)
    else()
      set(output_dir ${STK_CMAKE_${type}_OUTPUT_DIRECTORY})
    endif()
  endif()
  set(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
  if(NOT DEFINED STK_PLUGIN_${type}_OUTPUT_DIRECTORY)
    set(STK_PLUGIN_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
  endif()
endforeach()


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
include(CMake/stkFunctionCheckCompilerFlags.cmake)
#include(CMake/stkFunctionCheckoutRepo.cmake)
include(CMake/stkFunctionGetIncludeDirs.cmake)
include(CMake/stkFunctionGetLibraryDirs.cmake)
include(CMake/stkFunctionGetGccVersion.cmake)
include(CMake/stkFunctionGetCompilerVisibilityFlags.cmake)
#include(CMake/stkFunctionCompileSnippets.cmake)



#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
set(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
mark_as_advanced(ADDITIONAL_C_FLAGS)
set(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
mark_as_advanced(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set symbol visibility Flags
#

stkFunctionGetCompilerVisibilityFlags(VISIBILITY_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set coverage Flags
#
if(WITH_COVERAGE)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(coverage_flags "-g -fprofile-arcs -ftest-coverage -O0 -DNDEBUG")
    set(COVERAGE_CXX_FLAGS ${coverage_flags})
    set(COVERAGE_C_FLAGS ${coverage_flags})
  endif()
endif()

#-----------------------------------------------------------------------------
# STK C/CXX Flags
#
set(STK_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
set(STK_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCXX)
  set(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings")
  if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(cflags "${cflags} -D_FORTIFY_SOURCE=2")
  endif()

  stkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  stkFunctionCheckCompilerFlags("-Wl,--no-undefined" cflags)
  
  stkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)

  # With older version of gcc supporting the flag -fstack-protector-all, an extra dependency to libssp.so
  # is introduced. If gcc is smaller than 4.4.0 and the build type is Release let's not include the flag.
  # Doing so should allow to build package made for distribution using older linux distro.
  if(${GCC_VERSION} VERSION_GREATER "4.4.0" OR (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ${GCC_VERSION} VERSION_LESS "4.4.0"))
    stkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)
  endif()
  if(MINGW)
    # suppress warnings about auto imported symbols
    set(STK_CXX_FLAGS "-Wl,--enable-auto-import ${STK_CXX_FLAGS}")
  endif()

  set(STK_C_FLAGS "${cflags} ${STK_C_FLAGS}")
  set(STK_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${STK_CXX_FLAGS}")
endif()

if(MSVC)
  set(msvc_suppressed_warnings
    "/wd4290" # C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
  )
  set(STK_CXX_FLAGS "${STK_CXX_FLAGS} ${msvc_suppressed_warnings}")
endif()


# QT
#
stkMacroSetupQt()
# Update STK_BASE_LIBRARIES with QT libraries
if(QT4_FOUND)
  set(STK_BASE_LIBRARIES ${STK_BASE_LIBRARIES} ${QT_LIBRARIES} CACHE INTERNAL "STK base libraries" FORCE)
endif()
 
add_subdirectory(Libs/Hyperthermia)
add_subdirectory(Libs/RoboticSurgery)
add_subdirectory(Applications/stkRobotUR5Server)
