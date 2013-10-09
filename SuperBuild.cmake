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

include(ExternalProject)

#-----------------------------------------------------------------------------
# WARNING - No change should be required after this comment
#           when you are adding a new external project dependency.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Declare STK_EXTERNAL_LIBRARY_DIRS variable - This variable stores
# the library output directory associated with the different external project
# It's then used in CMake/LastConfigureStep/STKGenerateSTKConfig.cmake to
# configure STKConfig.cmake.in
# This variable would then be exposed to project building against STK
set(STK_EXTERNAL_LIBRARY_DIRS)

#-----------------------------------------------------------------------------
# Make sure ${STK_BINARY_DIR}/STK-build/bin exists
# May be used by some external project to install libs 
if(NOT EXISTS ${STK_BINARY_DIR}/STK-build/bin)
  file(MAKE_DIRECTORY ${STK_BINARY_DIR}/STK-build/bin)
endif()

#-----------------------------------------------------------------------------
# Qt is expected to be setup by STK/CMakeLists.txt just before it includes the SuperBuild script
#

#-----------------------------------------------------------------------------
# Attempt to discover Doxygen so that DOXYGEN_EXECUTABLE is set to an appropriate default value
#
find_package(Doxygen QUIET)

#-----------------------------------------------------------------------------
# Generate cmake variable name corresponding to Libs, Plugins and Applications
#
set(stk_libs_bool_vars)
foreach(lib ${STK_LIBS})
  list(APPEND stk_libs_bool_vars STK_LIB_${lib})
endforeach()

set(stk_plugins_bool_vars)
foreach(plugin ${STK_PLUGINS})
  list(APPEND stk_plugins_bool_vars STK_PLUGIN_${plugin})
endforeach()

set(stk_applications_bool_vars)
foreach(app ${STK_APPS})
  list(APPEND stk_applications_bool_vars STK_APP_${app})
endforeach()

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#

set(stk_cmake_boolean_args
  BUILD_TESTING
  STK_BUILD_ALL
  STK_BUILD_ALL_APPS
  STK_BUILD_ALL_LIBRARIES
  STK_BUILD_ALL_PLUGINS
  STK_BUILD_QTDESIGNER_PLUGINS
  STK_USE_QTTESTING
  STK_USE_KWSTYLE
  STK_USE_CONTRIBUTED_PLUGINS
  WITH_COVERAGE
  DOCUMENTATION_TARGET_IN_ALL
  STK_WRAP_PYTHONQT_FULL
  STK_ENABLE_Python_Wrapping
  ${stk_lib_options_list}
  )

set(stk_superbuild_boolean_args)
foreach(stk_cmake_arg ${stk_cmake_boolean_args})
  list(APPEND stk_superbuild_boolean_args -D${stk_cmake_arg}:BOOL=${${stk_cmake_arg}})
endforeach()

foreach(stk_cmake_arg ${stk_libs_bool_vars} ${stk_plugins_bool_vars} ${stk_applications_bool_vars})
  # Use the cached value of the option in case the current value has been
  # overridden by a "STK_BUILD_ALL" option.
  get_property(arg_value CACHE ${stk_cmake_arg} PROPERTY VALUE)
  list(APPEND stk_superbuild_boolean_args -D${stk_cmake_arg}:BOOL=${arg_value})
endforeach()

# message("CMake boolean args:")
# foreach(arg ${stk_superbuild_boolean_args})
#   message("  ${arg}")
# endforeach()

#-----------------------------------------------------------------------------
# Expand superbuild external project args
#
set(STK_SUPERBUILD_EP_ARGS)
set(STK_SUPERBUILD_EP_VARNAMES)
foreach(arg ${STK_SUPERBUILD_EP_VARS})
  string(REPLACE ":" ";" varname_and_vartype ${arg})
  set(target_info_list ${target_info_list})
  list(GET varname_and_vartype 0 _varname)
  list(GET varname_and_vartype 1 _vartype)
  list(APPEND STK_SUPERBUILD_EP_ARGS -D${_varname}:${_vartype}=${${_varname}})
  list(APPEND STK_SUPERBUILD_EP_VARNAMES ${_varname})
endforeach()
string(REPLACE ";" "^" STK_SUPERBUILD_EP_VARNAMES "${STK_SUPERBUILD_EP_VARNAMES}")

# message("CMake external project args:")
# foreach(arg ${STK_SUPERBUILD_EP_ARGS})
#   message("  ${arg}")
# endforeach()

#-----------------------------------------------------------------------------
if(STK_USE_CONTRIBUTED_PLUGINS)
  list(APPEND STK_SUPERBUILD_EP_ARGS
    -DExternalProjectsContrib_DIR:STRING=${ExternalProjectsContrib_DIR}
    -DPluginsContrib_DIR:STRING=${PluginsContrib_DIR}
    )
endif()

#-----------------------------------------------------------------------------
# STK Configure
#
set(proj STK-Configure)

ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DSTK_SUPERBUILD:BOOL=OFF
    -DSTK_SUPERBUILD_BINARY_DIR:PATH=${STK_BINARY_DIR}
    ${stk_superbuild_boolean_args}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS_INIT:STRING=${CMAKE_CXX_FLAGS_INIT}
    -DCMAKE_C_FLAGS_INIT:STRING=${CMAKE_C_FLAGS_INIT}
    -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
    -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
    -DSTK_BUILD_SHARED_LIBS:BOOL=${STK_BUILD_SHARED_LIBS}
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    -DDOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY:PATH=${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}
    -DDOXYGEN_EXECUTABLE:FILEPATH=${DOXYGEN_EXECUTABLE}
    -DSTK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${STK_CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DSTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${STK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DSTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${STK_CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DSTK_INSTALL_BIN_DIR:STRING=${STK_INSTALL_BIN_DIR}
    -DSTK_INSTALL_LIB_DIR:STRING=${STK_INSTALL_LIB_DIR}
    -DSTK_INSTALL_QTPLUGIN_DIR:STRING=${STK_INSTALL_QTPLUGIN_DIR}
    -DSTK_INSTALL_INCLUDE_DIR:STRING=${STK_INSTALL_INCLUDE_DIR}
    -DSTK_INSTALL_DOC_DIR:STRING=${STK_INSTALL_DOC_DIR}
    -DSTK_EXTERNAL_LIBRARY_DIRS:STRING=${STK_EXTERNAL_LIBRARY_DIRS}
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    ${STK_SUPERBUILD_EP_ARGS}
    -DSTK_SUPERBUILD_EP_VARNAMES:STRING=${STK_SUPERBUILD_EP_VARNAMES}
  SOURCE_DIR ${STK_SOURCE_DIR}
  BINARY_DIR ${STK_BINARY_DIR}/STK-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    ${STK_DEPENDENCIES}
  )

if(CMAKE_GENERATOR MATCHES ".*Makefiles.*")
  set(stk_build_cmd "$(MAKE)")
else()
  set(stk_build_cmd ${CMAKE_COMMAND} --build ${STK_BINARY_DIR}/STK-build --config ${CMAKE_CFG_INTDIR})
endif()

#-----------------------------------------------------------------------------
# STK
#
#message(STATUS SUPERBUILD_EXCLUDE_STKBUILD_TARGET:${SUPERBUILD_EXCLUDE_STKBUILD_TARGET})
if(NOT DEFINED SUPERBUILD_EXCLUDE_STKBUILD_TARGET OR NOT SUPERBUILD_EXCLUDE_STKBUILD_TARGET)
  set(STKBUILD_TARGET_ALL_OPTION "ALL")
else()
  set(STKBUILD_TARGET_ALL_OPTION "")
endif()

add_custom_target(STK-build ${STKBUILD_TARGET_ALL_OPTION}
  COMMAND ${stk_build_cmd}
  WORKING_DIRECTORY ${STK_BINARY_DIR}/STK-build
  )
add_dependencies(STK-build STK-Configure)

#-----------------------------------------------------------------------------
# Custom target allowing to drive the build of STK project itself
#
add_custom_target(STK
  COMMAND ${stk_build_cmd}
  WORKING_DIRECTORY ${STK_BINARY_DIR}/STK-build
  )
