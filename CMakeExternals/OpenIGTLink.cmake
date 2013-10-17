#
# OpenIGTLink
#

stkMacroShouldAddExternalproject(OpenIGTLink_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED OpenIGTLink_DIR AND NOT EXISTS ${OpenIGTLink_DIR})
    message(FATAL_ERROR "OpenIGTLink_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(OpenIGTLink_enabling_variable OpenIGTLink_LIBRARIES)

  set(proj OpenIGTLink)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})

  set(${OpenIGTLink_enabling_variable}_LIBRARY_DIRS OpenIGTLink_LIBRARY_DIRS)
  set(${OpenIGTLink_enabling_variable}_INCLUDE_DIRS OpenIGTLink_INCLUDE_DIRS)
  set(${OpenIGTLink_enabling_variable}_FIND_PACKAGE_CMD OpenIGTLink)

  if(STK_SUPERBUILD)

    if(NOT DEFINED OpenIGTLink_DIR)
    
      set(location_args )
      if(${proj}_URL)
        set(location_args URL ${${proj}_URL})
      else()
        set(location_args SVN_REPOSITORY "http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink")
      endif()
      
      set(ep_project_include_arg)
      if(CTEST_USE_LAUNCHERS)
        set(ep_project_include_arg
          "-DCMAKE_PROJECT_OpenIGTLink_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      endif()

    #   message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        DOWNLOAD_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          ${ep_project_include_arg}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(OpenIGTLink_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS OpenIGTLink_DIR:PATH)

  endif()

endif()
