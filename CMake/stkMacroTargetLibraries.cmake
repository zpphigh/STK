###########################################################################
#
#  Library:   STK
#
#  Copyright (c) Fitark.
#
###########################################################################

#!
#! This macro could be invoked using two different signatures:
#!   stkFunctionGetTargetLibraries(TARGET_LIBS)
#! or
#!   stkFunctionGetTargetLibraries(TARGET_LIBS "/path/to/stk_target_dir")
#!
#! Without specifying the second argument, the current folder will be used.
#!
#! \ingroup CMakeUtilities
function(stkFunctionGetTargetLibraries varname)

  set(expanded_target_library_list)

  set(TARGET_DIRECTORY ${ARGV1})
  if("${TARGET_DIRECTORY}" STREQUAL "")
    set(TARGET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  set(filepath ${TARGET_DIRECTORY}/target_libraries.cmake)
  set(manifestpath ${TARGET_DIRECTORY}/manifest_headers.cmake)

  # Check if "target_libraries.cmake" or "manifest_headers.cmake" file exists
  if(NOT EXISTS ${filepath} AND NOT EXISTS ${manifestpath})
    message(FATAL_ERROR "${filepath} or ${manifestpath} doesn't exists !")
  endif()

  # Make sure the variable is cleared
  set(target_libraries )
  set(Require-Plugin )

  if(EXISTS ${filepath})
    # Let's make sure target_libraries contains only strings
    file(STRINGS "${filepath}" stringtocheck) # read content of 'filepath' into 'stringtocheck'
    string(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    foreach(incorrect_element ${incorrect_elements})
      string(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      message(FATAL_ERROR "In ${filepath}, ${incorrect_element} should be replaced by ${correct_element}")
    endforeach()

    include(${filepath})

    # Loop over all target library, if it does *NOT* start with "STK",
    # let's resolve the variable to access its content
    foreach(target_library ${target_libraries})
      if(${target_library} MATCHES "^STK[a-zA-Z0-9]+$" OR
         ${target_library} MATCHES "^org_commontk_[a-zA-Z0-9_]+$")
        list(APPEND expanded_target_library_list ${target_library})
      else()
        list(APPEND expanded_target_library_list "${${target_library}}")
      endif()
    endforeach()
  endif()

  if(EXISTS ${manifestpath})
    # Let's make sure Require-Plugins contains only strings
    file(STRINGS "${manifestpath}" stringtocheck) # read content of 'manifestpath' into 'stringtocheck'
    string(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    foreach(incorrect_element ${incorrect_elements})
      string(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      message(FATAL_ERROR "In ${manifestpath}, ${incorrect_element} should be replaced by ${correct_element}")
    endforeach()

    include(${manifestpath})

    # Loop over all plugin dependencies,
    foreach(plugin_symbolicname ${Require-Plugin})
      string(REPLACE "." "_" plugin_library ${plugin_symbolicname})
      list(APPEND expanded_target_library_list ${plugin_library})
    endforeach()
  endif()
  
  # Pass the list of target libraries to the caller
  set(${varname} ${expanded_target_library_list} PARENT_SCOPE)

endfunction()

#! \ingroup CMakeUtilities
function(stkFunctionCollectTargetLibraryNames target_dir varname)

  set(target_library_list)
  #message(STATUS target:${target})
  set(lib_targets)

  set(filepath ${target_dir}/target_libraries.cmake)
  set(manifestpath ${target_dir}/manifest_headers.cmake)

  # Check if "target_libraries.cmake" or "manifest_headers.cmake" file exists
  if(NOT EXISTS ${filepath} AND NOT EXISTS ${manifestpath})
    message(FATAL_ERROR "${filepath} or ${manifestpath} doesn't exists !")
  endif()

  # Make sure the variable is cleared
  set(target_libraries )
  set(Require-Plugin )

  if(EXISTS ${filepath})
    # Let's make sure target_libraries contains only strings
    file(STRINGS "${filepath}" stringtocheck) # read content of 'filepath' into 'stringtocheck'
    string(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    foreach(incorrect_element ${incorrect_elements})
      string(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      message(FATAL_ERROR "In ${filepath}, ${incorrect_element} should be replaced by ${correct_element}")
    endforeach()

    include(${filepath})

    list(APPEND target_library_list ${target_libraries})
  endif()

  if(EXISTS ${manifestpath})
    # Let's make sure Require-Plugins contains only strings
    file(STRINGS "${manifestpath}" stringtocheck) # read content of 'manifestpath' into 'stringtocheck'
    string(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" incorrect_elements ${stringtocheck})
    foreach(incorrect_element ${incorrect_elements})
      string(REGEX REPLACE "\\$|\\{|\\}" "" correct_element ${incorrect_element})
      message(FATAL_ERROR "In ${manifestpath}, ${incorrect_element} should be replaced by ${correct_element}")
    endforeach()

    include(${manifestpath})

    # Loop over all plugin dependencies
    foreach(plugin_symbolicname ${Require-Plugin})
      string(REPLACE "." "_" plugin_library ${plugin_symbolicname})
      list(APPEND target_library_list ${plugin_library})
    endforeach()
  endif()

  list(REMOVE_DUPLICATES target_library_list)
  
  # Pass the list of target libraries to the caller
  set(${varname} ${target_library_list} PARENT_SCOPE)
endfunction()

#! \ingroup CMakeUtilities
macro(stkMacroCollectAllTargetLibraries targets subdir varname)

  set(option_prefix)
  if(${subdir} STREQUAL "Libs")
    set(option_prefix STK_LIB_)
  elseif(${subdir} STREQUAL "Plugins")
    set(option_prefix STK_PLUGIN_)
  elseif(${subdir} STREQUAL "Applications")
    set(option_prefix STK_APP_)
  else()
    message(FATAL_ERROR "Unknown subdir:${subdir}, expected value are: 'Libs, 'Plugins' or 'Applications'")
  endif()
  
  foreach(target ${targets})

    # Make sure the variable is cleared
    set(target_libraries )

    set(option_name ${option_prefix}${target})
    #message(STATUS option_name:${option_name})

    if(${target}_SOURCE_DIR)
      set(target_dir "${${target}_SOURCE_DIR}")
    else()
      set(target_dir "${STK_SOURCE_DIR}/${subdir}/${target}")
    endif()
    #message(STATUS target_dir:${target_dir})

    set(target_libraries)
    
    # Collect target libraries only if option is ON
    if(${option_name})
      stkFunctionCollectTargetLibraryNames(${target_dir} target_libraries)
    endif()

    if(target_libraries)
      list(APPEND ${varname} ${target_libraries})
      list(REMOVE_DUPLICATES ${varname})
    endif()
  endforeach()
  
endmacro()

#!
#! Extract all library names which are build within this project
#!
#! \ingroup CMakeUtilities
macro(stkMacroGetAllProjectTargetLibraries all_target_libraries varname)
  # Allow external projects to override the set of internal targets
  if(COMMAND GetMyTargetLibraries)
    GetMyTargetLibraries("${all_target_libraries}" ${varname})
  else()
    set(re_stklib "^(c|C)(t|T)(k|K)[a-zA-Z0-9]+$")
    set(re_stkplugin "^org_commontk_[a-zA-Z0-9_]+$")
    set(_tmp_list)
    list(APPEND _tmp_list ${all_target_libraries})
    #message("calling stkMacroListFilter with varname:${varname}")
    stkMacroListFilter(_tmp_list re_stklib re_stkplugin OUTPUT_VARIABLE ${varname})
    #message(STATUS "getallstklibs from ${all_target_libraries}")
    #message(STATUS varname:${varname}:${${varname}})
  endif()
endmacro()

#!
#! Extract all library names *NOT* being build within this project
#!
#! \ingroup CMakeUtilities
macro(stkMacroGetAllNonProjectTargetLibraries all_target_libraries varname)
  stkMacroGetAllProjectTargetLibraries("${all_target_libraries}" all_project_libraries)
  set(_tmp_list ${all_target_libraries})
  if(all_project_libraries)
    list(REMOVE_ITEM _tmp_list ${all_project_libraries})
  endif()
  set(${varname} ${_tmp_list})
  #message(varname:${varname}:${${varname}})
endmacro()

#! \ingroup CMakeUtilities
macro(stkMacroShouldAddExternalProject libraries_variable_name resultvar)
  set(${resultvar} FALSE)
  if(DEFINED NON_STK_DEPENDENCIES)
    list(FIND NON_STK_DEPENDENCIES ${libraries_variable_name} index)
  
    if(${index} GREATER -1)
      set(${resultvar} TRUE)
    endif()
  endif()
endmacro()
