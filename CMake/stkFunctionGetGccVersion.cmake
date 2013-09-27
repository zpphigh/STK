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
function(stkFunctionGetGccVersion path_to_gcc output_var)
  if(CMAKE_COMPILER_IS_GNUCXX)
    execute_process(
      COMMAND ${path_to_gcc} -dumpversion
      RESULT_VARIABLE result
      OUTPUT_VARIABLE output
      ERROR_VARIABLE error
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE
      )
    if(result)
      message(FATAL_ERROR "Failed to obtain compiler version running [${path_to_gcc} -dumpversion]: ${error}")
    endif()
    set(${output_var} ${output} PARENT_SCOPE)
  endif()
endfunction()
