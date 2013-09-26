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

project(STKRoboticSurgery)

# CMake modules
set(CMAKE_MODULE_PATH ${stkRoboticSurgery_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
set(KIT_export_directive "STK_ROBOTICSURGERY_EXPORT")


# Source files
set(KIT_SRCS
  stkRobotSimulator.h
  stkRobotSimulator.cpp
  stkRobotServer.h
  stkRobotServer.cpp
  )


# Headers that should run through moc
set(KIT_MOC_SRCS
  stkRobotSimulator.h
  stkRobotServer.h
  )


# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
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
