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

project(STKSlicerExt)

#
# 3rd party dependencies
#
include(${VTK_USE_FILE})
include(${ITK_USE_FILE})
include(${Slicer_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
set(KIT_export_directive "STK_SLICEREXT_EXPORT")

# Source files
set(KIT_SRCS
  stkSlicerDisplayHelper.h
  stkSlicerDisplayHelper.cpp
  stkSlicerVolumeNodeDisplayHelper.h
  stkSlicerVolumeNodeDisplayHelper.cpp
  stkMRMLCrossPointNode.h
  stkMRMLCrossPointNode.cpp
  stkMRMLHelper.h
  stkMRMLHelper.cpp
  stkSlicerHelper.h
  stkSlicerHelper.cpp
  stkMeasureWidget.h
  stkMeasureWidget.cpp
  stkProbeWidget.h
  stkProbeWidget.cpp
  stkMeasureRepresentation.h
  stkMeasureRepresentation.cpp
  stkProbeRepresentation3D.h
  stkProbeRepresentation3D.cpp
  stkMRMLProbeDisplayableManager.h
  stkMRMLProbeDisplayableManager.cpp
  stkMRMLProbeNode.h
  stkMRMLProbeNode.cpp
  stkMRMLProbeStorageNode.h
  stkMRMLProbeStorageNode.cpp
  stkSlicerLayoutWidget.h
  stkSlicerLayoutWidget.cpp
  stkSlicerDataWidget.h
  stkSlicerDataWidget.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  stkSlicerLayoutWidget.h
  stkSlicerDataWidget.h
  )

# UI files
set(KIT_UI_FORMS
  stkSlicerLayoutWidget.ui
  stkSlicerDataWidget.ui
)

# Resources
set(KIT_resources
	Resources/stkSlicerExt.qrc
)

# Includes
set(KIT_INCLUDE_DIRECTORIES
	${MRMLCore_INCLUDE_DIRS}
	${Slicer_Libs_INCLUDE_DIRS}
    ${Slicer_Base_INCLUDE_DIRS}
    ${Slicer_ModuleLogic_INCLUDE_DIRS}
    ${Slicer_ModuleMRML_INCLUDE_DIRS} 
	${Slicer_ModuleWidgets_INCLUDE_DIRS} 
    ${KIT_INCLUDE_DIRECTORIES}
)

include_directories(${KIT_INCLUDE_DIRECTORIES})

stkFunctionGetTargetLibraries(KIT_target_libraries)

set(KIT_target_libraries
  MRMLDisplayableManager
  MRMLLogic
  RemoteIO
  MRMLCore
  qSlicerModulesCore
  qSlicerBaseQTGUI
  qSlicerBaseQTCore
  qSlicerBaseQTApp
  SlicerBaseLogic
  vtkSlicerVolumesModuleLogic
  vtkSlicerVolumeRenderingModuleLogic
  vtkSlicerVolumeRenderingModuleMRML
  vtkSlicerAnnotationsModuleLogic
  vtkSlicerAnnotationsModuleMRML
  vtkSlicerAnnotationsModuleMRMLDisplayableManager
  ${KIT_target_libraries}
)

stkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  #INCLUDE_DIRECTORIES {KIT_INCLUDE_DIRECTORIES}
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
