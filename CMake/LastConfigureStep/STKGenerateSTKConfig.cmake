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

#
# Generate the STKConfig.cmake file in the build tree. Also configure
# one for installation.  The file tells external projects how to use STK.
#

include(stkFunctionGeneratePluginUseFile)

# Construct version numbers for STKConfigVersion.cmake.
set(_STK_VERSION_MAJOR ${STK_MAJOR_VERSION})
set(_STK_VERSION_MINOR ${STK_MINOR_VERSION})
set(_STK_VERSION_PATCH ${STK_PATCH_VERSION})
# We use odd minor numbers for development versions.
# Use a date for the development patch level.
# if("${_STK_VERSION_MINOR}" MATCHES "[13579]$")
#   include(${STK_SOURCE_DIR}/Utilities/kwsys/kwsysDateStamp.cmake)
#   set(_STK_VERSION_PATCH
#     "${KWSYS_DATE_STAMP_YEAR}${KWSYS_DATE_STAMP_MONTH}${KWSYS_DATE_STAMP_DAY}"
#     )
# endif()

#-----------------------------------------------------------------------------
# Settings shared between the build tree and install tree.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
set(STK_CONFIG_INSTALL_ONLY)

# The "use" file.
set(STK_USE_FILE ${STK_SUPERBUILD_BINARY_DIR}/UseSTK.cmake)

# Generate list of target to exports
set(STK_TARGETS_TO_EXPORT ${STK_LIBRARIES} ${STK_PLUGIN_LIBRARIES})

# Append STK PythonQt static libraries
if(NOT STK_BUILD_SHARED_LIBS)
  foreach(lib ${STK_WRAPPED_LIBRARIES_PYTHONQT})
    list(APPEND STK_TARGETS_TO_EXPORT ${lib}PythonQt)
  endforeach()
endif()

# Export targets so they can be imported by a project using STK
# as an external library
export(TARGETS ${STK_TARGETS_TO_EXPORT} FILE ${STK_SUPERBUILD_BINARY_DIR}/STKExports.cmake)

# Generate a file containing plugin specific variables
set(STK_PLUGIN_USE_FILE "${STK_SUPERBUILD_BINARY_DIR}/STKPluginUseFile.cmake")
stkFunctionGeneratePluginUsefile(${STK_PLUGIN_USE_FILE})

# Write a set of variables containing library specific include and library directories
set(STK_LIBRARY_INCLUDE_DIRS_CONFIG)
foreach(lib ${STK_LIBRARIES})
  set(${lib}_INCLUDE_DIRS ${${lib}_SOURCE_DIR} ${${lib}_BINARY_DIR})
  stkFunctionGetIncludeDirs(${lib}_INCLUDE_DIRS ${lib})
  set(STK_LIBRARY_INCLUDE_DIRS_CONFIG "${STK_LIBRARY_INCLUDE_DIRS_CONFIG}
set(${lib}_INCLUDE_DIRS \"${${lib}_INCLUDE_DIRS}\")")

  stkFunctionGetLibraryDirs(${lib}_LIBRARY_DIRS ${lib})
  set(STK_LIBRARY_LIBRARY_DIRS_CONFIG "${STK_LIBRARY_LIBRARY_DIRS_CONFIG}
set(${lib}_LIBRARY_DIRS \"${${lib}_LIBRARY_DIRS}\")")
endforeach()

# Determine the include directories needed.
set(STK_INCLUDE_DIRS_CONFIG
  ${STK_BASE_INCLUDE_DIRS}
)

set(STKTesting_CMAKE_DIR_CONFIG "${STKTesting_SOURCE_DIR}/CMake")

# Consider "headeronly" libraries
set(headeronly_libs STKTesting)
foreach(lib ${headeronly_libs})
  set(${lib}_INCLUDE_DIRS ${${lib}_SOURCE_DIR} ${${lib}_BINARY_DIR})
  stkFunctionGetIncludeDirs(${lib}_INCLUDE_DIRS ${lib})
  set(STK_LIBRARY_INCLUDE_DIRS_CONFIG "${STK_LIBRARY_INCLUDE_DIRS_CONFIG}
set(${lib}_INCLUDE_DIRS \"${${lib}_INCLUDE_DIRS}\")")
  set(STK_INCLUDE_DIRS_CONFIG ${STK_INCLUDE_DIRS_CONFIG} ${${lib}_INCLUDE_DIRS})
endforeach()



# Library directory.
set(STK_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Plug-in output directory
if(WIN32)
  set(_plugin_output_type "RUNTIME")
else()
  set(_plugin_output_type "LIBRARY")
endif()
if(DEFINED STK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY)
  if(IS_ABSOLUTE "${STK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
    set(STK_PLUGIN_LIBRARIES_DIR_CONFIG "${STK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
  else()
    set(STK_PLUGIN_LIBRARIES_DIR_CONFIG "${CMAKE_${_plugin_output_type}_OUTPUT_DIRECTORY}/${STK_PLUGIN_${_plugin_output_type}_OUTPUT_DIRECTORY}")
  endif()
else()
  set(STK_PLUGIN_LIBRARIES_DIR_CONFIG "${CMAKE_${_plugin_output_type}_OUTPUT_DIRECTORY}/plugins")
endif()

# External project libraries.
set(STK_EXTERNAL_LIBRARIES_CONFIG ${STK_EXTERNAL_LIBRARIES})

# External project library directory.
set(STK_EXTERNAL_LIBRARY_DIRS_CONFIG ${STK_EXTERNAL_LIBRARY_DIRS})

# Runtime library directory.
set(STK_RUNTIME_LIBRARY_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Binary executable directory.
set(STK_EXECUTABLE_DIRS_CONFIG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# QtDesigner plugins directory
set(STK_QTDESIGNERPLUGINS_DIR_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# STK external projects variables
string(REPLACE "^" ";" STK_SUPERBUILD_EP_VARNAMES "${STK_SUPERBUILD_EP_VARNAMES}")
set(STK_SUPERBUILD_EP_VARS_CONFIG)
foreach(varname ${STK_SUPERBUILD_EP_VARNAMES})
  set(STK_SUPERBUILD_EP_VARS_CONFIG
   "${STK_SUPERBUILD_EP_VARS_CONFIG}
set(STK_${varname} \"${${varname}}\")")
endforeach()

# Executable locations.

# CMake extension module directory.
set(STK_CMAKE_DIR_CONFIG ${STK_CMAKE_DIR})
set(STK_CMAKE_UTILITIES_DIR_CONFIG ${STK_CMAKE_UTILITIES_DIR})

# Build configuration information.
set(STK_CONFIGURATION_TYPES_CONFIG ${CMAKE_CONFIGURATION_TYPES})
set(STK_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
# Configure STKConfig.cmake for the build tree.
configure_file(${STK_SOURCE_DIR}/CMake/STKConfig.cmake.in
               ${STK_SUPERBUILD_BINARY_DIR}/STKConfig.cmake @ONLY IMMEDIATE)
configure_file(${STK_SOURCE_DIR}/CMake/STKConfigVersion.cmake.in
               ${STK_SUPERBUILD_BINARY_DIR}/STKConfigVersion.cmake @ONLY IMMEDIATE)
configure_file(${STK_SOURCE_DIR}/CMake/stkConfig.h.in
               ${STK_CONFIG_H_INCLUDE_DIR}/stkConfig.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

#-----------------------------------------------------------------------------
# Configure STKConfig.cmake for the install tree.
