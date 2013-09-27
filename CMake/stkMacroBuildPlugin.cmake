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
#
# Depends on:
#  STK/CMake/stkMacroParseArguments.cmake
#  STK/CMake/stkMacroGeneratePluginManifest.cmake
#
#! \brief Build a STK plug-in.
#!
#! This macro takes the usual arguments for building
#! a shared library using Qt. Additionally, it generates
#! plugin meta-data by creating a MANIFEST.MF text file
#! which is embedded in the share library as a Qt resource.
#!
#! The following variables can be set in a file named
#! manifest_headers.cmake, which will then be read by
#! this macro:
#!
#! - Plugin-ActivationPolicy
#! - Plugin-Category
#! - Plugin-ContactAddress
#! - Plugin-Copyright
#! - Plugin-Description
#! - Plugin-DocURL
#! - Plugin-Icon
#! - Plugin-License
#! - Plugin-Name
#! - Require-Plugin
#! - Plugin-Vendor
#! - Plugin-Version
#!
#! \ingroup CMakeAPI
macro(stkMacroBuildPlugin)
  CtkMacroParseArguments(MY
    "EXPORT_DIRECTIVE;SRCS;MOC_SRCS;MOC_OPTIONS;UI_FORMS;INCLUDE_DIRECTORIES;EXPORTED_INCLUDE_SUFFIXES;TARGET_LIBRARIES;RESOURCES;CACHED_RESOURCEFILES;TRANSLATIONS;OUTPUT_DIR"
    "TEST_PLUGIN"
    ${ARGN}
    )

  # Sanity checks
  if(NOT DEFINED MY_EXPORT_DIRECTIVE)
    message(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  endif()
  
  # Print a warning if the project name does not match the directory name
  get_filename_component(_dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  string(REPLACE "." "_" _dir_name_with_ ${_dir_name})
  if(NOT _dir_name_with_ STREQUAL ${PROJECT_NAME})
    message(WARNING "Discouraged mismatch of plug-in project name [${PROJECT_NAME}] and top-level directory name [${CMAKE_CURRENT_SOURCE_DIR}].")
  endif()

  # Define library name
  set(lib_name ${PROJECT_NAME})
  
  # Plug-in target names must contain at leas one _
  if(NOT lib_name MATCHES _)
    message(FATAL_ERROR "The plug-in project name ${lib_name} must contain at least one '_' character")
  endif()
  
  # Plugin are expected to be shared library
  set(MY_LIBRARY_TYPE "SHARED")

  # Clear the variables for the manifest headers
  set(Plugin-ActivationPolicy )
  set(Plugin-Category )
  set(Plugin-ContactAddress )
  set(Plugin-Copyright )
  set(Plugin-Description )
  set(Plugin-DocURL )
  set(Plugin-Icon )
  set(Plugin-License )
  set(Plugin-Name )
  set(Require-Plugin )
  set(Plugin-SymbolicName )
  set(Plugin-Vendor )
  set(Plugin-Version )

  set(Custom-Headers )

  if(MY_TEST_PLUGIN)
    # Since the test plug-ins are not considered when calculating
    # target dependencies via DGraph, we add the dependencies
    # manually here
    #message("${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES}")
    list(APPEND ${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES})
  endif()

  # If a file named manifest_headers.cmake exists, read it
  set(manifest_headers_dep )
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
    include(${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake)
    set(manifest_headers_dep "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
  endif()

  string(REPLACE "_" "." Plugin-SymbolicName ${lib_name})

  # --------------------------------------------------------------------------
  # Include dirs
  if(MY_EXPORTED_INCLUDE_SUFFIXES)
    set(${lib_name}_INCLUDE_SUFFIXES ${MY_EXPORTED_INCLUDE_SUFFIXES}
        CACHE INTERNAL "List of exported plugin include dirs")

    set(my_includes )
    foreach(_suffix ${MY_EXPORTED_INCLUDE_SUFFIXES})
      list(APPEND my_includes ${CMAKE_CURRENT_SOURCE_DIR}/${_suffix})
    endforeach()
  else()
    set(${lib_name}_INCLUDE_SUFFIXES ""
        CACHE INTERNAL "List of exported plugin include dirs")

    set(my_includes )
  endif()

  list(APPEND my_includes
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${MY_INCLUDE_DIRECTORIES}
      )

  # Add the include directories from the plugin dependencies
  # and external dependencies
  stkFunctionGetIncludeDirs(my_includes ${lib_name})

  include_directories(
    ${my_includes}
    )

  # Add Qt include dirs and defines
  include(${QT_USE_FILE})

  # Add the library directories from the external project
  stkFunctionGetLibraryDirs(my_library_dirs ${lib_name})

  link_directories(
    ${my_library_dirs}
    )

  set(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  set(MY_EXPORT_HEADER_PREFIX "${lib_name}_")
  set(MY_LIBNAME ${lib_name})

  configure_file(
    ${STK_EXPORT_HEADER_TEMPLATE}
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
      QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src} ${MY_MOC_OPTIONS})
    endforeach()
  endif()
  QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  if(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  endif()

  # Add the generated manifest qrc file
  set(manifest_qrc_src )
  stkFunctionGeneratePluginManifest(manifest_qrc_src
    ACTIVATIONPOLICY ${Plugin-ActivationPolicy}
    CATEGORY ${Plugin-Category}
    CONTACT_ADDRESS ${Plugin-ContactAddress}
    COPYRIGHT ${Plugin-Copyright}
    DESCRIPTION ${Plugin-Description}
    DOC_URL ${Plugin-DocURL}
    ICON ${Plugin-Icon}
    LICENSE ${Plugin-License}
    NAME ${Plugin-Name}
    REQUIRE_PLUGIN ${Require-Plugin}
    SYMBOLIC_NAME ${Plugin-SymbolicName}
    VENDOR ${Plugin-Vendor}
    VERSION ${Plugin-Version}
    CUSTOM_HEADERS ${Custom-Headers}
    )

  if(manifest_headers_dep)
    set_property(SOURCE ${manifest_qrc_src} APPEND
                   PROPERTY OBJECT_DEPENDS ${manifest_headers_dep})
  endif()
  list(APPEND MY_QRC_SRCS ${manifest_qrc_src})

  # Create translation files (.ts and .qm)
  set(_plugin_qm_files )
  set(_plugin_cached_resources_in_binary_tree )
  set(_translations_dir "${CMAKE_CURRENT_BINARY_DIR}/STK-INF/l10n")
  if(MY_TRANSLATIONS)
    set_source_files_properties(${MY_TRANSLATIONS}
                                PROPERTIES OUTPUT_LOCATION ${_translations_dir})
    QT4_CREATE_TRANSLATION(_plugin_qm_files ${MY_SRCS} ${MY_UI_FORMS} ${MY_TRANSLATIONS})
  endif()

  if(_plugin_qm_files)
    foreach(_qm_file ${_plugin_qm_files})
      file(RELATIVE_PATH _relative_qm_file ${CMAKE_CURRENT_BINARY_DIR} ${_qm_file})
      list(APPEND _plugin_cached_resources_in_binary_tree ${_relative_qm_file})
    endforeach()
  endif()

  set(_plugin_cached_resources_in_source_tree )
  if(MY_CACHED_RESOURCEFILES)
    foreach(_cached_resource ${MY_CACHED_RESOURCEFILES})
      if(IS_ABSOLUTE "${_cached_resource}")
        # create a path relative to the current binary dir
        file(RELATIVE_PATH _relative_cached_resource ${CMAKE_CURRENT_BINARY_DIR} ${_cached_resource})
        list(APPEND _plugin_cached_resources_in_binary_tree ${_relative_cached_resource})
      else()
        list(APPEND _plugin_cached_resources_in_source_tree ${_cached_resource})
      endif()
    endforeach()
  endif()

  # Add any other additional resource files
  if(_plugin_cached_resources_in_source_tree OR _plugin_cached_resources_in_binary_tree)
    string(REPLACE "." "_" _plugin_symbolicname ${Plugin-SymbolicName})
    stkMacroGeneratePluginResourcefile(MY_QRC_SRCS
      NAME ${_plugin_symbolicname}_cached.qrc
      PREFIX ${Plugin-SymbolicName}
      RESOURCES ${_plugin_cached_resources_in_source_tree}
      BINARY_RESOURCES ${_plugin_cached_resources_in_binary_tree})
  endif()

  source_group("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    ${MY_TRANSLATIONS}
    )

  source_group("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${_plugin_qm_files}
    )

  add_library(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    ${_plugin_qm_files}
    )

  # Set the output directory for the plugin
  if(MY_OUTPUT_DIR)
    set(output_dir_suffix "/${MY_OUTPUT_DIR}")
  else()
    set(output_dir_suffix "")
  endif()

  foreach(type RUNTIME LIBRARY ARCHIVE)
    if(NOT DEFINED STK_PLUGIN_${type}_OUTPUT_DIRECTORY AND CMAKE_${type}_OUTPUT_DIRECTORY)
      # Put plug-ins by default into a "plugins" subdirectory
      set(STK_PLUGIN_${type}_OUTPUT_DIRECTORY "${CMAKE_${type}_OUTPUT_DIRECTORY}/plugins")
    endif()
    
    if(IS_ABSOLUTE "${STK_PLUGIN_${type}_OUTPUT_DIRECTORY}")
      set(plugin_${type}_output_dir "${STK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    elseif(CMAKE_${type}_OUTPUT_DIRECTORY)
      set(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/${STK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    else()
      set(plugin_${type}_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${STK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    endif()

    if(MY_TEST_PLUGIN)
      # Test plug-ins will always be put in a separate directory
      if(CMAKE_${type}_OUTPUT_DIRECTORY)
        set(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/test_plugins")
      else()
        set(plugin_${type}_output_dir "${PROJECT_BINARY_DIR}/test_plugins")
      endif()
    endif()
  endforeach()

  set(plugin_compile_flags "-DQT_PLUGIN")
  stkFunctionGetCompilerVisibilityFlags(plugin_compile_flags)

  # Apply properties to the library target.
  set_target_properties(${lib_name} PROPERTIES
    COMPILE_FLAGS "${plugin_compile_flags}"
    RUNTIME_OUTPUT_DIRECTORY ${plugin_RUNTIME_output_dir}
    LIBRARY_OUTPUT_DIRECTORY ${plugin_LIBRARY_output_dir}
    ARCHIVE_OUTPUT_DIRECTORY ${plugin_ARCHIVE_output_dir}
    PREFIX "lib"
    )

  # Note: The plugin may be installed in some other location ???
  # Install rules
# if(MY_LIBRARY_TYPE STREQUAL "SHARED")
# install(TARGETS ${lib_name}
# RUNTIME DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
# LIBRARY DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
# ARCHIVE DESTINATION ${STK_INSTALL_LIB_DIR} COMPONENT Development)
# endif()

  set(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  if(MINGW)
    list(APPEND my_libs ssp) # add stack smash protection lib
  endif()

  target_link_libraries(${lib_name} ${my_libs})

  if(NOT MY_TEST_PLUGIN)
    set(${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES} ${lib_name} CACHE INTERNAL "STK plugins" FORCE)
  endif()

  # Install headers
  #file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
  #install(FILES
  # ${headers}
  # ${dynamicHeaders}
  # DESTINATION ${STK_INSTALL_INCLUDE_DIR} COMPONENT Development
  # )

endmacro()

