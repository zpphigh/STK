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

#
# 3rd party dependencies
#
include(${IGSTK_USE_FILE})
include(${Slicer_USE_FILE})


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
	stkAuroraTracker.h
	stkAuroraTracker.cpp
	stkAuroraTrackerTool.h
	stkAuroraTrackerTool.cpp
	stkAuroraTrackerWidget.h
	stkAuroraTrackerWidget.cpp
	stkPolarisTracker.h
	stkPolarisTracker.cpp
	stkPolarisTrackerTool.h
	stkPolarisTrackerTool.cpp
	stkPolarisTrackerWidget.h
	stkPolarisTrackerWidget.cpp
	stkPivotCalibrationWidget.h
	stkPivotCalibrationWidget.cpp	
	stkPolarisPivotCalibrationWidget.h
	stkPolarisPivotCalibrationWidget.cpp
	stkFiducialMarkerRegistrationWidget.h
	stkFiducialMarkerRegistrationWidget.cpp
	stkIGTLCircularBuffer.h
	stkIGTLCircularBuffer.cpp
	stkIGTLToMRMLBase.h
	stkIGTLToMRMLBase.cpp
  )


# Headers that should run through moc
set(KIT_MOC_SRCS
	stkTracker.h
	stkTrackerTool.h
	stkAuroraTracker.h
	stkAuroraTrackerTool.h
	stkAuroraTrackerWidget.h
	stkPolarisTracker.h
	stkPolarisTrackerTool.h
	stkPolarisTrackerWidget.h
	stkPivotCalibrationWidget.h
	stkPolarisPivotCalibrationWidget.h
	stkFiducialMarkerRegistrationWidget.h
  )


# UI files
set(KIT_UI_FORMS
	stkAuroraTrackerWidget.ui
	stkPolarisTrackerWidget.ui
	stkPivotCalibrationWidget.ui
	stkPolarisPivotCalibrationWidget.ui
	stkFiducialMarkerRegistrationWidget.ui
)

# Resources
set(KIT_resources
	Resources/stkSurgeryNavigation.qrc
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

# Target libraries - See CMake/stkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries)

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
  vtkSlicerMarkupsModuleMRML
  ${KIT_target_libraries}
)

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
