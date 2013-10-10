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

project(STKSurgeryNavigation)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(IGSTK REQUIRED)
include(${IGSTK_USE_FILE})

message("OpenIGTLink_USE_FILE is: " ${OpenIGTLink_USE_FILE})
message("IGSTK_USE_FILE is: " ${IGSTK_USE_FILE})

# CMake modules
set(CMAKE_MODULE_PATH ${STKSurgeryNavigation_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
set(KIT_export_directive "STK_SURGERYNAVIGATION_EXPORT")


# Source files
set(KIT_SRCS
   stkTracker.h
   stkTracker.cpp
   stkTrackerTool.h
   stkTrackerTool.cpp
   stkTrackingErrorObserver.h
   stkTrackingErrorObserver.cpp
  )


# Headers that should run through moc
set(KIT_MOC_SRCS
   stkTracker.h
   stkTrackerTool.h
  )


# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)


# Target libraries - See CMake/stkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries OpenIGTLink)

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
