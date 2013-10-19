#
# zlib
#
stkMacroShouldAddExternalproject(zlib_LIBRARIES add_project)
if(${add_project})
 
  message("zlib superbuild")
  # Sanity checks  
  if(DEFINED zlib_DIR AND NOT EXISTS ${zlib_DIR})
    message(FATAL_ERROR "zlib_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(zlib_enabling_variable zlib_LIBRARIES)

  set(proj zlib)
  set(proj_DEPENDENCIES "")

  list(APPEND STK_DEPENDENCIES ${proj})

  set(${zlib_enabling_variable}_LIBRARY_DIRS zlib_LIBRARY_DIRS)
  set(${zlib_enabling_variable}_INCLUDE_DIRS zlib_INCLUDE_DIRS)
  set(${zlib_enabling_variable}_FIND_PACKAGE_CMD zlib)

  if(STK_SUPERBUILD)

    if(NOT DEFINED zlib_DIR)

		if(NOT CMAKE_CONFIGURATION_TYPES)
			list(APPEND EXTERNAL_PROJECT_OPTIONAL_ARGS
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
		endif()

		if(NOT DEFINED git_protocol)
			set(git_protocol "git")
		endif()
	
		set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/zlib)
		set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/zlib-build)
		set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/zlib-install)
      
  #     message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build
        PREFIX ${proj}${ep_suffix}
		GIT_REPOSITORY "${git_protocol}://github.com/commontk/zlib.git"
		GIT_TAG "66a753054b356da85e1838a081aa94287226823e"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
			${ep_common_cache_args}
			${ep_project_include_arg}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(zlib_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

	  set(STK_ZLIB_ROOT ${zlib_DIR})
	  set(STK_ZLIB_INCLUDE_DIR ${zlib_DIR}/include )
	  set(STK_ZLIB_LIBRARY_DIR ${zlib_DIR}/lib)
	  if(WIN32)
		set(STK_ZLIB_LIBRARY     ${SLICER_ZLIB_LIBRARY_DIR}/zlib.lib )
	  else()
		set(STK_ZLIB_LIBRARY     ${SLICER_ZLIB_LIBRARY_DIR}/libzlib.a )
      endif()
      # Since the link directories associated with zlib is used, it makes sens to
      # update STK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
      list(APPEND STK_EXTERNAL_LIBRARY_DIRS ${zlib_DIR}/bin)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS zlib_DIR:PATH)

  endif()

endif()

