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

project(STKHyperthermia)

#
# 3rd party dependencies
#
include(${VTK_USE_FILE})
include(${ITK_USE_FILE})

# CMake modules
set(CMAKE_MODULE_PATH ${stkHyperthermia_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
set(KIT_export_directive "STK_HYPERTHERMIA_EXPORT")


# Source files
set(KIT_SRCS
  stkThermalField.h
  stkThermalField.cpp
  )


# Headers that should run through moc
set(KIT_MOC_SRCS
  stkThermalField.h
  )


# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

# Set VTK_LIBRARIES variable
set(VTK_LIBRARIES
  vtkCommon
  vtkFiltering
  vtkRendering
  vtkHybrid
  )


# Target libraries - See CMake/stkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries)

stkFunctionGetTargetLibraries(KIT_target_libraries)

stkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${STK_LIBRARY_MODE}
  )

# Needed for ctkBackTrace
if(UNIX)
  target_link_libraries(${PROJECT_NAME} dl)
elseif(WIN32 AND NOT MINGW)
  target_link_libraries(${PROJECT_NAME} dbghelp)
endif()

# Testing
if(BUILD_TESTING)
 # add_subdirectory(Testing)
endif()
