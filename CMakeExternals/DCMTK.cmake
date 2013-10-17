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
      set(revision_tag ae3b946f6e6231)
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
        set(location_args GIT_REPOSITORY "${git_protocol}://git.dcmtk.org/dcmtk.git"
                          GIT_TAG ${revision_tag})
      endif()
      
      set(ep_project_include_arg)
      if(CTEST_USE_LAUNCHERS)
        set(ep_project_include_arg
          "-DCMAKE_PROJECT_DCMTK_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      endif()

      #message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        DOWNLOAD_COMMAND ""
        CMAKE_GENERATOR ${gen}
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        CMAKE_ARGS
          -DDCMTK_INSTALL_BINDIR:STRING=bin/${CMAKE_CFG_INTDIR}
          -DDCMTK_INSTALL_LIBDIR:STRING=lib/${CMAKE_CFG_INTDIR}
        CMAKE_CACHE_ARGS
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
        )
      set(DCMTK_DIR ${ep_install_dir})

  # This was used during heavy development on DCMTK itself.
  # Disabling it for now. (It also leads to to build errors
  # with the XCode CMake generator on Mac).
  #
  #    ExternalProject_Add_Step(${proj} force_rebuild
  #      COMMENT "Force ${proj} re-build"
  #      DEPENDERS build    # Steps that depend on this step
  #      ALWAYS 1
  #      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${proj}-build
  #      DEPENDS
  #        ${proj_DEPENDENCIES}
  #      )

      # Since DCMTK is statically build, there is not need to add its corresponding
      # library output directory to STK_EXTERNAL_LIBRARY_DIRS

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS DCMTK_DIR:PATH)

  endif()

endif()
