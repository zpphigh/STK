###########################################################################
#
#  Library:   STK
#
#  Copyright (c) Fitark.
#
###########################################################################

#
# Depends on:
#  STK/CMake/stkMacroParseArguments.cmake
#

#! \brief Build a STK library.
#!
#! \ingroup CMakeAPI
macro(stkMacroBuildLib)
  stkMacroParseArguments(MY
    "NAME;EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES;LIBRARY_TYPE"
    "ENABLE_QTTESTING"
    ${ARGN}
    )

  # Sanity checks
  if(NOT DEFINED MY_NAME)
    message(FATAL_ERROR "NAME is mandatory")
  endif()
  string(REGEX MATCH "^STK.+" valid_library_name ${MY_NAME})
  if(NOT valid_library_name)
    message(FATAL_ERROR "STK library name [${MY_NAME}] should start with 'STK' uppercase !")
  endif()
  if(NOT DEFINED MY_EXPORT_DIRECTIVE)
    message(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  endif()
  if(NOT DEFINED MY_LIBRARY_TYPE)
    set(MY_LIBRARY_TYPE "SHARED")
  endif()

  # Define library name
  set(lib_name ${MY_NAME})
  
  # Library target names must not contain a '_' (reserved for plug-in target names)
  if(lib_name MATCHES _)
    message(FATAL_ERROR "The library name ${lib_name} must not contain a '_' character.")
  endif()

  # --------------------------------------------------------------------------
  # Include dirs
  set(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    # with CMake >2.9, use QT4_MAKE_OUTPUT_FILE instead ?
    ${CMAKE_CURRENT_BINARY_DIR}/Resources/UI
    ${MY_INCLUDE_DIRECTORIES}
    )

  # Add the include directories from the library dependencies
  stkFunctionGetIncludeDirs(my_includes ${lib_name})

  include_directories(
    ${my_includes}
    )
  #message(lib_name:${lib_name})
  #foreach(i ${my_includes})
  #  message(i:${i})
  #endforeach()

  # Add the library directories from the external project
  stkFunctionGetLibraryDirs(my_library_dirs ${lib_name})

  link_directories(
    ${my_library_dirs}
    )

  set(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  set(MY_EXPORT_HEADER_PREFIX ${MY_NAME})
  string(REGEX REPLACE "^STK" "stk" MY_EXPORT_HEADER_PREFIX ${MY_EXPORT_HEADER_PREFIX})
  set(MY_LIBNAME ${lib_name})

  configure_file(
    ${STK_SOURCE_DIR}/Libs/stkExport.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  set(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  set(MY_MOC_CPP)
  set(MY_UI_CPP)
  set(MY_QRC_SRCS)

  # Wrap
  if(MY_MOC_SRCS)
    # this is a workaround for Visual Studio. The relative include paths in the generated
    # moc files can get very long and can't be resolved by the MSVC compiler.
    foreach(moc_src ${MY_MOC_SRCS})
      QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src})
    endforeach()
  endif()
  QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  if(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  endif()

  source_group("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  source_group("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MOC_CPP_DECORATOR}
    )

  add_library(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    )

  # Set labels associated with the target.
  set_target_properties(${lib_name} PROPERTIES LABELS ${lib_name})

  # Apply user-defined properties to the library target.
  if(STK_LIBRARY_PROPERTIES AND MY_LIBRARY_TYPE STREQUAL "SHARED")
    set_target_properties(${lib_name} PROPERTIES ${STK_LIBRARY_PROPERTIES})
  endif()
  set_target_properties(${lib_name} PROPERTIES STK_LIB_TARGET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  # Library properties specific to STATIC build
  if(MY_LIBRARY_TYPE STREQUAL "STATIC")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set_target_properties(${lib_name} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
  endif()

  # Install rules
  if(MY_LIBRARY_TYPE STREQUAL "SHARED")
    install(TARGETS ${lib_name}
      RUNTIME DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      LIBRARY DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT Development)
  endif()

  set(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  if(MINGW)
    list(APPEND my_libs ssp) # add stack smash protection lib
  endif()
  target_link_libraries(${lib_name} ${my_libs})

  # Update STK_BASE_LIBRARIES
  set(STK_BASE_LIBRARIES ${my_libs} ${lib_name} CACHE INTERNAL "STK base libraries" FORCE)
  set(STK_LIBRARIES ${STK_LIBRARIES} ${lib_name} CACHE INTERNAL "STK libraries" FORCE)
  set(STK_BASE_INCLUDE_DIRS ${STK_BASE_INCLUDE_DIRS} ${my_includes} CACHE INTERNAL "STK includes" FORCE)

  # Install headers
  file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
  install(FILES
    ${headers}
    ${dynamicHeaders}
    DESTINATION ${STK_INSTALL_INCLUDE_DIR} COMPONENT Development
    )

endmacro()


