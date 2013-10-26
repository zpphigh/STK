#cmake_minimum_required(VERSION 2.8.7)
project(VolumeDisplayController)
#
##-----------------------------------------------------------------------------
#if(NOT Slicer_SOURCE_DIR)
#  set(EXTENSION_NAME VolumeViewController)
#  set(EXTENSION_HOMEPAGE "https://github.com/Fitark/STK")
#  set(EXTENSION_CATEGORY "STK")
#  set(EXTENSION_CONTRIBUTORS "Weiming Zhai")
#  set(EXTENSION_DESCRIPTION "Simplified Volume Display Controller for 3D Slicer")
#  set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/SurgeryNavigation/Resources/Icons/SurgeryNavigation.png?revision=19437&view=co")
#  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-SurgeryNavigation-screenshot.png")
#endif()

find_package(STK REQUIRED)
include(${STK_USE_FILE})

#find_package(Slicer REQUIRED)

include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
set(MODULE_NAME VolumeDisplayController)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(Logic)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${Slicer_ModuleLogic_INCLUDE_DIRS}
  ${Slicer_ModuleMRML_INCLUDE_DIRS} 
  ${Slicer_ModuleWidgets_INCLUDE_DIRS} 
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  qSlicer${MODULE_NAME}ModuleWidgets
  qMRMLWidgets
  qSlicerVolumesModuleWidgets
  vtkSlicerVolumeRenderingModuleLogic
  qSlicerVolumeRenderingModuleWidgets
  STKSlicerExt
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildQtModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

