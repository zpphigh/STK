

macro(stkMacroSetupExternalPlugins )
  message(SEND_ERROR "This macro has been renamed. Please use stkMacroSetupPlugins instead")
endmacro()

#! This is the main macro to set up your STK plug-ins inside your own CMake project.
#!
#! This macro takes care of validating the current set of plug-in build options,
#! enables and/or checks required plug-ins and handles all aspects of plug-in
#! dependencies. Additionally, it calls add_subdirectory() on each given plug-in.
#!
#! Macro signature:
#! \code
#! stkMacroSetupPlugins(plugins...
#!                      [BUILD_OPTION_PREFIX <option_prefix>]
#!                      [APPS <apps...>]
#!                      [BUILD_ALL <build_all_flag>]
#!                      [COMPACT_OPTIONS])
#! \endcode
#!
#! \param plugins (required) A list of directories (absolute or relative to the current
#!        source dir) with default build options. E.g. an item of the list may look like
#!        "Plugins/org.myproject.example:ON"
#! \param BUILD_OPTION_PREFIX (optional) The prefix to use for the build option of th
#!        plug-in. Defaults to "BUILD_".
#! \param APPS (optional) A list of directories with build option names containing
#!        STK-style applications. This can be used to automatically enable plug-ins
#!        required by the application. The application directoy must contain a CMakeLists.txt
#!        file containing a "project(...)" command and a target_libraries.cmake file
#!        containing a list of required plug-in targets, e.g. "set(target_libraries org_myproject_example)".
#!        An item of the list may look like "Apps/MyExampleApp^^BUILD_APP_MyExampleApp" where
#!        Apps/MyExampleApp is the directory containing the applicatios CMakeLists.txt file
#!        and BUILD_APP_MyExampleApp is the build option used to conditionally build the app.
#! \param BUILD_ALL (optional) If the build_all_flag is true, all entries in the plugins list
#!        will be enabled (but their build option will not be forced to change). Default is OFF
#! \param COMPACT_OPTIONS (optional) If this flag is given, the created build options for the
#!        plugins will not contain the relative path but just the last directory entry which
#!        usually is the plug-in symbolic name.
#!
#! Example invocation:
#!
#! \code
#! set(plugin_list Plugins/org.myproject.example:OFF)
#! set(app_list Apps/MyExampleApp^^MYPROJECT_BUILD_MyExampleApp)
#!
#! stkMacroSetupPlugins(${plugin_list}
#!                      BUILD_OPTION_PREFIX MYPROJECT_BUILD_
#!                      APPS ${app_list}
#!                      COMPACT_OPTIONS
#!                     )
#! \endcode
#!
#! \ingroup CMakeAPI
macro(stkMacroSetupPlugins )

  stkMacroParseArguments(MY "BUILD_OPTION_PREFIX;APPS;BUILD_ALL" "COMPACT_OPTIONS" ${ARGN})

  if(NOT MY_DEFAULT_ARGS)
    message(FATAL_ERROR "Empty plugin list")
  endif()

  set(plugin_list ${MY_DEFAULT_ARGS})

  if(NOT MY_BUILD_OPTION_PREFIX)
    set(MY_BUILD_OPTION_PREFIX "BUILD_")
  endif()

  if(NOT MY_BUILD_ALL)
    set(MY_BUILD_ALL 0)
  endif()
  
  # Check if this is the first invocation of this macro
  get_property(_repeated GLOBAL PROPERTY stkMacroSetupExternalPlugins_called SET)
  if(NOT _repeated)
    # Clear the internal cache variable containing all enabled plug-in targets
    # This variable will be set in stkMacroBuildPlugin
    set(${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES CACHE INTERNAL "STK plug-in targets" FORCE)
    set_property(GLOBAL PROPERTY stkMacroSetupExternalPlugins_called 1)
    
    # Add the project specific variable name containing plug-in targets to the list
    set_property(GLOBAL APPEND PROPERTY STK_PLUGIN_LIBRARIES_VARS ${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES)
  endif()
  
  # Set up Qt, if not already done
  if(NOT QT4_FOUND)
    set(minimum_required_qt_version "4.6")
    find_package(Qt4 REQUIRED)

    if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "${minimum_required_qt_version}")
      message(FATAL_ERROR "error: STK requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
    endif()
  endif()
  
  # Set the variable QT_INSTALLED_LIBRARY_DIR that contains all
  # Qt shared libraries
  set(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
  if(WIN32)
    get_filename_component(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
  endif()

  set(plugin_symbolic_names )
  set(plugin_targets )
  foreach(plugin ${plugin_list})
    stkFunctionExtractOptionNameAndValue(${plugin} plugin_dir plugin_value)
    string(REPLACE "/" ";" _tokens ${plugin_dir})
    list(GET _tokens -1 plugin_symbolic_name)
    list(APPEND plugin_symbolic_names ${plugin_symbolic_name})
    
    string(REPLACE "." "_" plugin_target ${plugin_symbolic_name})
    list(APPEND plugin_targets ${plugin_target})
    
    set(${plugin_symbolic_name}_plugin_dir ${plugin_dir})
    set(${plugin_symbolic_name}_plugin_value ${plugin_value})
  endforeach()

  # Check if the plugin symbolic names are valid for the current project
  stkMacroGetAllNonProjectTargetLibraries("${plugin_targets}" invalid_targets)
  if(invalid_targets)
    set(invalid_plugins )
    foreach(plugin_target ${invalid_targets})
      string(REPLACE "_" "." plugin_symbolic_name ${plugin_target})
      list(APPEND invalid_plugins ${plugin_symbolic_name})
    endforeach()
    message(FATAL_ERROR "The following plug-ins are using invalid symbolic names: ${invalid_plugins}")
  endif()
  
  set(plugin_dirswithoption )
  set(plugin_subdirs )
  foreach(plugin_symbolic_name ${plugin_symbolic_names})
    if(MY_COMPACT_OPTIONS)
      set(option_name ${MY_BUILD_OPTION_PREFIX}${plugin_symbolic_name})
    else()
      set(option_name ${MY_BUILD_OPTION_PREFIX}${${plugin_symbolic_name}_plugin_dir})
    endif()
    # This variable may have the form "Plugins/org.commontk.bla_option_name"
    set(${${plugin_symbolic_name}_plugin_dir}_option_name ${option_name})
    # Additionally create a variable of the form "org_commontk_bla_option_name"
    string(REPLACE "." "_" plugin_target ${plugin_symbolic_name})
    set(${plugin_target}_option_name ${option_name})

    option(${option_name} "Build the ${plugin_symbolic_name} Plugin." ${${plugin_symbolic_name}_plugin_value})
    if(MY_BUILD_ALL)
      set(${option_name} 1)
    endif()

    list(APPEND plugin_subdirs "${${plugin_symbolic_name}_plugin_dir}")
    if(IS_ABSOLUTE ${${plugin_symbolic_name}_plugin_dir})
      list(APPEND plugin_dirswithoption "${${plugin_symbolic_name}_plugin_dir}^^${option_name}")
    else()
      list(APPEND plugin_dirswithoption "${CMAKE_CURRENT_SOURCE_DIR}/${${plugin_symbolic_name}_plugin_dir}^^${option_name}")
    endif()
  endforeach()
  
  # Get plugin info from possible previous invocations of this macro for
  # validation purposes below
  get_property(previous_plugin_dirswithoption GLOBAL PROPERTY stkMacroSetupExternalPlugins_dirswithoption)
  
  # Fill the STK_EXTERNAL_PLUGIN_LIBRARIES variable with external plug-in target names.
  # It will be used in stkMacroValidateBuildOptions to be able to validate agains plug-ins
  # from external projects.
  stkFunctionGetAllPluginTargets(STK_EXTERNAL_PLUGIN_LIBRARIES)

  stkFunctionGenerateDGraphInput(${CMAKE_CURRENT_BINARY_DIR}
                                 "${plugin_dirswithoption};${previous_plugin_dirswithoption};${MY_APPS}"
                                 WITH_EXTERNALS)
  stkMacroValidateBuildOptions("${CMAKE_CURRENT_BINARY_DIR}" "${STK_DGRAPH_EXECUTABLE}"
                               "${MY_APPS};${plugin_dirswithoption};${previous_plugin_dirswithoption}")

  # Record the current set of plug-ins and their option names
  set_property(GLOBAL APPEND PROPERTY stkMacroSetupExternalPlugins_dirswithoption ${plugin_dirswithoption})

  # Get the gcc version (GCC_VERSION will be empty if the compiler is not gcc).
  # This will be used in the stkMacroBuildPlugin macro to conditionally set compiler flags.
  stkFunctionGetGccversion(${CMAKE_CXX_COMPILER} GCC_VERSION)

  foreach(plugin ${plugin_subdirs})
    if(${${plugin}_option_name})
      if(IS_ABSOLUTE ${plugin})
        # get last directory component
        get_filename_component(_dirname ${plugin} NAME) 
        add_subdirectory(${plugin} private_plugins/${_dirname})
      else()
        add_subdirectory(${plugin})
      endif()
    endif()
  endforeach()

endmacro()
