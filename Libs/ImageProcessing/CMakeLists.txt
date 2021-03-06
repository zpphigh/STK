###########################################################################
#
#  Library:   STK
#
#  Copyright (c) fitark.org
#
#	Tsinghua University
#	Department of Interventional Ultrasound, Chinese PLA General Hospital
#
###########################################################################

project(STKImageProcessing)

#
# 3rd party dependencies
#
include(${ITK_USE_FILE})
include(${VTK_USE_FILE})
include(${Slicer_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#
set(KIT_export_directive "STK_IMAGEPROCESSING_EXPORT")

# Source files
set(KIT_SRCS
  stkImageHelper.h
  stkImageHelper.cpp
  )


# Headers that should run through moc
set(KIT_MOC_SRCS
  stkImageHelper.h
  )


# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
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
