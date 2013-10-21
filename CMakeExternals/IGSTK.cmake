#
# IGSTK
#

stkMacroShouldAddExternalproject(IGSTK_LIBRARIES add_project)
if(${add_project})
  
  message("IGSTK SuperBuild")

  # Sanity checks
  if(DEFINED IGSTK_DIR AND NOT EXISTS ${IGSTK_DIR})
    message(FATAL_ERROR "IGSTK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(IGSTK_enabling_variable IGSTK_LIBRARIES)

  set(proj IGSTK)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})
  list(APPEND proj_DEPENDENCIES ITK)

  set(${IGSTK_enabling_variable}_INCLUDE_DIRS IGSTK_INCLUDE_DIR)
  set(${IGSTK_enabling_variable}_FIND_PACKAGE_CMD IGSTK)

  if(STK_SUPERBUILD)

    if(NOT DEFINED IGSTK_DIR)
     
      set(ep_project_include_arg)
      if(CTEST_USE_LAUNCHERS)
        set(ep_project_include_arg
          "-DCMAKE_PROJECT_IGSTK_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      endif()

      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY git://igstk.org/IGSTK.git
		GIT_TAG bb7dfd4683026a
	    DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
          -DIGSTK_INSTALL_BINDIR:STRING=bin/${CMAKE_CFG_INTDIR}
          -DIGSTK_INSTALL_LIBDIR:STRING=lib/${CMAKE_CFG_INTDIR}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          ${ep_project_include_arg}
          -DBUILD_TESTING:BOOL=ON 
          -DBUILD_SHARED_LIBS:BOOL=OFF
          -DIGSTK_USE_OpenIGTLink:BOOL=ON
          -DIGSTK_USE_Qt:BOOL=OFF 
          -DDCMTK_DIR:PATH=${DCMTK_DIR}
          -DITK_DIR:PATH=${ITK_DIR}
          -DVTK_DIR:PATH=${VTK_DIR}
          -DOpenIGTLink_DIR:PATH=${OpenIGTLink_DIR}
          #-DIGSTK_USE_Ascension3DGTracker:BOOL=ON 
          #-DIGSTK_TEST_Ascension3DGTracker_ATTACHED:BOOL=ON
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(IGSTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS IGSTK_DIR:PATH)

  endif()

endif()
