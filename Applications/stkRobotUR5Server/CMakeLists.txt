project(stkRobotUR5Server)

#
# See STK/CMake/stkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  stkRobotUR5ServerMain.cpp
  stkRobotUR5ServerWidget.cpp
  stkRobotUR5ServerWidget.h
)


# Headers that should run through moc
set(KIT_MOC_SRCS
  stkRobotUR5ServerWidget.h
)

# UI files
set(KIT_UI_FORMS
  stkRobotUR5ServerWidget.ui
)


# Resources
set(KIT_resources
  stkRobotUR5Server.qrc
)

# Target libraries - See CMake/stkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
stkFunctionGetTargetLibraries(KIT_target_libraries)
  
# Additional directories to include - Note that CTK_INCLUDE_LIBRARIES is already included
set(KIT_include_directories
)

# Fix this after having discussed include dependencies with Jc
foreach(_dep ${${PROJECT_NAME}_DEPENDENCIES})
  include_directories(${${_dep}_SOURCE_DIR} ${${_dep}_BINARY_DIR})
endforeach()

stkMacroBuildApp(
  NAME ${PROJECT_NAME}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )
