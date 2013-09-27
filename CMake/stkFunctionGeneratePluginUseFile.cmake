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


#! Write a set of variables containing STK plugin specific include and library directories
#! \ingroup CMakeAPI
function(stkFunctionGeneratePluginUseFile filename)
  set(STK_PLUGIN_INCLUDE_DIRS_CONFIG)

  foreach(plugin ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES})
    set(${plugin}_INCLUDE_DIRS )

    # The call to stkFunctionGetIncludeDirs returns all include dirs
    # the plugin itself needs. This does not include the plugin's
    # source dir, so we add it explicitly.
    if(${plugin}_INCLUDE_SUFFIXES)
      foreach(_suffix ${${plugin}_INCLUDE_SUFFIXES})
        list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR}/${_suffix})
      endforeach()
    else()
      list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_SOURCE_DIR})
    endif()

    list(APPEND ${plugin}_INCLUDE_DIRS ${${plugin}_BINARY_DIR})

    stkFunctionGetIncludeDirs(${plugin}_INCLUDE_DIRS ${plugin})

    set(STK_PLUGIN_INCLUDE_DIRS_CONFIG "${STK_PLUGIN_INCLUDE_DIRS_CONFIG}
set(${plugin}_INCLUDE_DIRS \"${${plugin}_INCLUDE_DIRS}\")")

    stkFunctionGetLibraryDirs(${plugin}_LIBRARY_DIRS ${plugin})
    set(STK_PLUGIN_LIBRARY_DIRS_CONFIG "${STK_PLUGIN_LIBRARY_DIRS_CONFIG}
set(${plugin}_LIBRARY_DIRS \"${${plugin}_LIBRARY_DIRS}\")")
  endforeach()
  
  set(_stk_plugin_libraries_variable ${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES)
  set(_stk_plugin_libraries ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES})

  configure_file("${STK_CMAKE_DIR}/../CMake/STKPluginUseFile.cmake.in" "${filename}" @ONLY)
endfunction()
