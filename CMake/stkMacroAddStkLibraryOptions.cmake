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
macro(stkMacroAddStkLibraryOptions lib)

  set(filepath ${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}/stk_library_options.cmake)

  # Add options only if "stk_library_option.cmake" file exists
  if(EXISTS ${filepath})
  
    # Make sure the variable is cleared 
    set(stk_library_options )

    include(${filepath})

    foreach(option ${stk_library_options})
      stkFunctionExtractOptionNameAndValue(${option} option_name option_value)
      option(STK_LIB_${lib}_${option_name} "Enable ${lib} Library ${option_name} option." ${option_value})
      mark_as_advanced(STK_LIB_${lib}_${option_name})
      list(APPEND stk_lib_options_list STK_LIB_${lib}_${option_name})
    endforeach()
    
  endif()
  
endmacro()
