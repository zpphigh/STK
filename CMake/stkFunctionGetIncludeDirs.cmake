###########################################################################
#
#  Library: STK
#
#  Copyright (c) Fitark.
#
###########################################################################

#! \ingroup CMakeUtilities
function(stkFunctionGetIncludeDirs var_include_dirs)

  if(NOT ARGN)
    message(FATAL_ERROR "No targets given")
  endif()

  set(_include_dirs ${${var_include_dirs}} ${STK_CONFIG_H_INCLUDE_DIR})
  foreach(_target ${ARGN})

    # Add the include directories from the plugin dependencies
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro stkMacroValidateBuildOptions

    set(stk_deps )
    set(ext_deps )

    stkMacroGetAllProjectTargetLibraries("${${_target}_DEPENDENCIES}" stk_deps)
    stkMacroGetAllNonProjectTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)

    foreach(dep ${stk_deps})

      if(${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      else()
        list(APPEND _include_dirs
             ${${dep}_SOURCE_DIR}
             ${${dep}_BINARY_DIR}
             )
      endif()

      # For external projects, STKConfig.cmake contains variables
      # listening the include dirs for STK libraries and plugins
      if(${dep}_INCLUDE_DIRS)
        list(APPEND _include_dirs ${${dep}_INCLUDE_DIRS})
      endif()
    endforeach()

    foreach(dep ${ext_deps})

      if(${dep}_INCLUDE_DIRS)
        string(REPLACE "^" ";" _ext_include_dirs "${${dep}_INCLUDE_DIRS}")
        list(APPEND _include_dirs ${_ext_include_dirs})
      endif()

      # This is for resolving include dependencies between
      # libraries / plugins from external projects using STK
      if(${dep}_SOURCE_DIR AND ${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      elseif(${dep}_SOURCE_DIR)
        list(APPEND _include_dirs ${${dep}_SOURCE_DIR})
      endif()

      if(${dep}_BINARY_DIR)
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      endif()

    endforeach()

  endforeach()

  if(_include_dirs)
    list(REMOVE_DUPLICATES _include_dirs)
  endif()
  set(${var_include_dirs} ${_include_dirs} PARENT_SCOPE)

endfunction()
