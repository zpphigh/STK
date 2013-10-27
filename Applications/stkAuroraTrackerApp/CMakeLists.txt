project(stkAuroraTrackerApp)

#
# 3rd party dependencies
#
include(${IGSTK_USE_FILE})
include(${Slicer_USE_FILE})

#
# See STK/CMake/stkMacroBuildApp.cmake for details
#

# Source files
set(KIT_SRCS
  stkAuroraTrackerAppMain.cpp
)


# Headers that should run through moc
set(KIT_MOC_SRCS
)

# UI files
set(KIT_UI_FORMS
)


#include_directories
set(${PROJECT_NAME}_DEPENDENCIES
  STKSurgeryNavigation
  STKSlicerExt
)

# Fix this after having discussed include dependencies with Jc
foreach(_dep ${${PROJECT_NAME}_DEPENDENCIES})
  include_directories(${${_dep}_SOURCE_DIR} ${${_dep}_BINARY_DIR})
endforeach()


# Target libraries - See CMake/stkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
stkFunctionGetTargetLibraries(KIT_target_libraries)
  

stkMacroBuildApp(
  NAME ${PROJECT_NAME}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )
