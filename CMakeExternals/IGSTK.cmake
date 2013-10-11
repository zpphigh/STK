#
# IGSTK
#

stkMacroShouldAddExternalproject(IGSTK_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED IGSTK_DIR AND NOT EXISTS ${IGSTK_DIR})
    message(FATAL_ERROR "IGSTK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(IGSTK_enabling_variable IGSTK_LIBRARIES)

  set(proj IGSTK)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})

  set(${IGSTK_enabling_variable}_INCLUDE_DIRS IGSTK_INCLUDE_DIR)
  set(${IGSTK_enabling_variable}_FIND_PACKAGE_CMD IGSTK)

  if(STK_SUPERBUILD)

    if(NOT DEFINED IGSTK_DIR)
      set(revision_tag bb7dfd4683026a)					   
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()
      
      set(location_args )
      if(${proj}_URL)
        set(location_args URL ${${proj}_URL})
      elseif(${proj}_GIT_REPOSITORY)
        set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                          GIT_TAG ${revision_tag})
      else()
        set(location_args GIT_REPOSITORY "${git_protocol}://igstk.org/IGSTK.git"
                          GIT_TAG ${revision_tag})
      endif()
      
      set(ep_project_include_arg)
      if(CTEST_USE_LAUNCHERS)
        set(ep_project_include_arg
          "-DCMAKE_PROJECT_IGSTK_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      endif()

      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
	    DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
          #-DIGSTK_INSTALL_BINDIR:STRING=bin/${CMAKE_CFG_INTDIR}
          #-DIGSTK_INSTALL_LIBDIR:STRING=lib/${CMAKE_CFG_INTDIR}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          ${ep_project_include_arg}
          -DBUILD_SHARED_LIBS:BOOL=OFF
          -DIGSTK_USE_OpenIGTLink:BOOL=ON
          -DIGSTK_USE_Qt:BOOL=ON 
        )
      set(IGSTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS IGSTK_DIR:PATH)

  endif()

endif()