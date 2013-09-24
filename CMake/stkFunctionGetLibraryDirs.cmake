###########################################################################
#
#  Library: STK
#
#  Copyright (c) Fitark
#
###########################################################################

#! \ingroup CMakeUtilities
function(stkFunctionGetLibraryDirs var_library_dirs)

  if(NOT ARGN)
    message(FATAL_ERROR "No targets given")
  endif()

  set(_library_dirs ${${var_library_dirs}})
  foreach(_target ${ARGN})

    # Add the library directories from the external project
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro stkMacroValidateBuildOptions

    set(ext_deps )

    stkMacroGetAllNonProjectTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)
    
    foreach(dep ${ext_deps})

      if(${dep}_LIBRARY_DIRS)
        string(REPLACE "^" ";" _ext_library_dirs "${${dep}_LIBRARY_DIRS}")
        list(APPEND _library_dirs ${_ext_library_dirs})
      endif()

    endforeach()

  endforeach()

  if(_library_dirs)
    list(REMOVE_DUPLICATES _library_dirs)
  endif()
  set(${var_library_dirs} ${_library_dirs} PARENT_SCOPE)

endfunction()
