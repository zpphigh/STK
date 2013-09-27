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

#!
#! Depending on the platform, either PATH, LD_LIBRARY_PATH or DYLD_LIBRARY_PATH
#! will be updated using the paths specified using PATH_LIST
#!
#! \ingroup CMakeUtilities
macro(stkMacroSetPaths PATH_LIST)
  foreach(p ${PATH_LIST})
    if(WIN32)
      set(ENV{PATH} "${p};$ENV{PATH}")
    elseif(APPLE)
      set(ENV{DYLD_LIBRARY_PATH} "${p}:$ENV{DYLD_LIBRARY_PATH}")
    else()
      set(ENV{LD_LIBRARY_PATH} "${p}:$ENV{LD_LIBRARY_PATH}")
    endif()
  endforeach()
endmacro()
