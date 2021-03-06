#
# ITK
#
stkMacroShouldAddExternalproject(ITK_LIBRARIES add_project)
if(${add_project})
 

  # Sanity checks
  if(DEFINED ITK_DIR AND NOT EXISTS ${ITK_DIR})
    message(FATAL_ERROR "ITK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(ITK_enabling_variable ITK_LIBRARIES)

  set(proj ITK)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})
  #list(APPEND proj_DEPENDENCIES DCMTK)

  set(${ITK_enabling_variable}_LIBRARY_DIRS ITK_LIBRARY_DIRS)
  set(${ITK_enabling_variable}_INCLUDE_DIRS ITK_INCLUDE_DIRS)
  set(${ITK_enabling_variable}_FIND_PACKAGE_CMD ITK)

  if(STK_SUPERBUILD)

    if(NOT DEFINED ITK_DIR)

#     message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY git://itk.org/ITK.git
		GIT_TAG 237932867d8491db6a7745e6b4cc2b25f8e4470c
        #DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
			${ep_common_cache_args}
			${ep_project_include_arg}
			-DBUILD_TESTING:BOOL=OFF
			-DBUILD_EXAMPLES:BOOL=OFF
			-DITK_LEGACY_REMOVE:BOOL=OFF
			-DITKV3_COMPATIBILITY:BOOL=ON
			-DITK_BUILD_ALL_MODULES:BOOL=ON
			-DModule_ITKReview:BOOL=ON
			-DBUILD_SHARED_LIBS:BOOL=ON
			-DITK_WRAPPING:BOOL=OFF 
			#DCMTK
			#-DITK_USE_SYSTEM_DCMTK:BOOL=ON
			#-DDCMTK_DIR:PATH=${DCMTK_DIR}
			#-DModule_ITKIODCMTK:BOOL=ON			
			 # ZLIB
			#-DITK_USE_SYSTEM_ZLIB:BOOL=ON
			#-DZLIB_ROOT:PATH=${STK_ZLIB_ROOT}
			#-DZLIB_INCLUDE_DIR:PATH=${STK_ZLIB_INCLUDE_DIR}
			#-DZLIB_LIBRARY:FILEPATH=${STK_ZLIB_LIBRARY}
			#-DITK_USE_REVIEW:BOOL=ON
			#-DITK_USE_REVIEW_STATISTICS:BOOL=ON
			#-DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON
			#-DITK_USE_PORTABLE_ROUND:BOOL=ON
			#-DITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY:BOOL=ON
			#-DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(ITK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

      # Since the link directories associated with ITK is used, it makes sens to
      # update STK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
      list(APPEND STK_EXTERNAL_LIBRARY_DIRS ${ITK_DIR}/bin)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS ITK_DIR:PATH)

  endif()

endif()

