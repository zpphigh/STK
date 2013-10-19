#
# See CMake/stkFunctionGetTargetLibraries.cmake
#
# This file should list the libraries required to build the current STK libraries
#

set(target_libraries
  QT_LIBRARIES
  zlib_LIBRARIES
  OpenIGTLink_LIBRARIES
  IGSTK_LIBRARIES
  ITK_LIBRARIES
  DCMTK_LIBRARIES
  )
