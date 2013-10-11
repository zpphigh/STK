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


# The commands in this directory are intended to be executed at
# the end of the whole configuration process, as a "last step".
# This directory is typically the last SUBDIRS in the main CMakeLists.txt.
# It enable the above commands to use variables that might have been configured
# in previous SUBDIRS. This is especially important when it comes to
# the CONFIGURE_FILE command, since in IMMEDIATE mode that command will
# use the current values of CMake variables instead of waiting until the
# end of CMakeLists processing, i.e. instead of waiting until some variables
# are configured in SUBDIRS.


set(stk_target_dependencies)

# Loop over the list of STK* targets and retrieve the associated dependencies
foreach(stk_target ${CTEST_PROJECT_SUBPROJECTS})
  list(APPEND stk_target_dependencies ${${stk_target}_LIB_DEPENDS})
endforeach()


set(STK_EXTERNAL_LIBRARIES)

# Loop over dependencies and append to STK_EXTERNAL_LIBRARIES 
# target dependency which are neither STK or static library
set(link_type)
foreach(stk_target_dependency ${stk_target_dependencies})
  string(REGEX MATCH "^(general|optimized|debug)$" is_link_type ${stk_target_dependency})
  if(is_link_type)
    set(link_type ${stk_target_dependency})
  else()
    # Sanity checks - link_type shouldn't be empty
    if(link_type STREQUAL "")
      message(SEND_ERROR "link_type shouldn't be empty")
    endif()
    # Make sure stk_target_dependency is not a STK library
    string(REGEX MATCH "(^(c|C)(t|T)(k|K)[a-zA-Z0-9]+$)" is_stk_library ${stk_target_dependency})
    if(NOT is_stk_library)
      # Make sure stk_target_dependency is not a static library
      get_filename_component(stk_target_dependency_ext ${stk_target_dependency} EXT)
      if(NOT stk_target_dependency_ext STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX)
        list(APPEND STK_EXTERNAL_LIBRARIES ${link_type} ${stk_target_dependency})
      endif()
      set(link_type)
    endif()
  endif()
endforeach()

# Clean variable
list(REMOVE_DUPLICATES STK_BASE_INCLUDE_DIRS)

#-----------------------------------------------------------------------------
# Create the STKConfig.cmake file containing the STK configuration.
# Since it might generate configuration file depending
include(STKGenerateSTKConfig.cmake)
