#
# DCMTK
#

stkMacroShouldAddExternalproject(DCMTK_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED DCMTK_DIR AND NOT EXISTS ${DCMTK_DIR})
    message(FATAL_ERROR "DCMTK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(DCMTK_enabling_variable DCMTK_LIBRARIES)

  set(proj DCMTK)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})

  set(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
  set(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)

  if(STK_SUPERBUILD)

    if(NOT DEFINED DCMTK_DIR)
      
      set(ep_project_include_arg)
      if(CTEST_USE_LAUNCHERS)
        set(ep_project_include_arg
          "-DCMAKE_PROJECT_DCMTK_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      endif()

      message("Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY git://github.com/commontk/DCMTK.git
        GIT_TAG "f461865d1759854db56e4c840991c81c77e45bb9"
        #DOWNLOAD_COMMAND ""
        CMAKE_GENERATOR ${gen}
        UPDATE_COMMAND ""
        #BUILD_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
          -DDCMTK_INSTALL_BINDIR:STRING=bin/${CMAKE_CFG_INTDIR}
          -DDCMTK_INSTALL_LIBDIR:STRING=lib/${CMAKE_CFG_INTDIR}
        CMAKE_ARGS
          ${ep_common_cache_args}
          ${ep_project_include_arg}
          -DBUILD_SHARED_LIBS:BOOL=OFF
          -DDCMTK_WITH_DOXYGEN:BOOL=OFF
          -DDCMTK_WITH_ZLIB:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_OPENSSL:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_PNG:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_TIFF:BOOL=OFF  # see github issue #25
          -DDCMTK_WITH_XML:BOOL=OFF  # see github issue #25
          -DDCMTK_WITH_ICONV:BOOL=OFF  # see github issue #178
          -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
          -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(DCMTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS DCMTK_DIR:PATH)

  endif()

endif()
