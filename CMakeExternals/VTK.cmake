#
# VTK
#

stkMacroShouldAddExternalproject(VTK_LIBRARIES add_project)
if(${add_project} OR STK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  # Sanity checks
  if(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
    message(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(VTK_enabling_variable VTK_LIBRARIES)

  set(proj VTK)
  set(proj_DEPENDENCIES)

  list(APPEND STK_DEPENDENCIES ${proj})

  set(${VTK_enabling_variable}_LIBRARY_DIRS VTK_LIBRARY_DIRS)
  set(${VTK_enabling_variable}_INCLUDE_DIRS VTK_INCLUDE_DIRS)
  set(${VTK_enabling_variable}_FIND_PACKAGE_CMD VTK)

  if(STK_SUPERBUILD)

    set(additional_vtk_cmakevars )
    if(MINGW)
      list(APPEND additional_vtk_cmakevars -DCMAKE_USE_PTHREADS:BOOL=OFF)
    endif()

    if(STK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
      list(APPEND additional_vtk_cmakevars
        -DPYTHON_EXECUTABLE:PATH=${PYTHON_EXECUTABLE}
        -DPYTHON_LIBRARIES:FILEPATH=${PYTHON_LIBRARIES}
        -DPYTHON_DEBUG_LIBRARIES:FILEPATH=${PYTHON_DEBUG_LIBRARIES}
        )
    endif()

    if(NOT DEFINED VTK_DIR)

      set(revision_tag v5.10.0)
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
        set(location_args GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
                          GIT_TAG ${revision_tag})
      endif()

  #     message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          ${additional_vtk_cmakevars}
          -DVTK_WRAP_TCL:BOOL=OFF
          -DVTK_USE_TK:BOOL=OFF
          -DVTK_WRAP_PYTHON:BOOL=${STK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK}
          -DVTK_WRAP_JAVA:BOOL=OFF
          -DBUILD_SHARED_LIBS:BOOL=ON
          -DDESIRED_QT_VERSION:STRING=4
          -DVTK_USE_GUISUPPORT:BOOL=ON
          -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
          -DVTK_USE_QT:BOOL=ON
          -DVTK_LEGACY_REMOVE:BOOL=ON
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

      # Since the link directories associated with VTK is used, it makes sens to
      # update STK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
      list(APPEND STK_EXTERNAL_LIBRARY_DIRS ${VTK_DIR}/bin)

    else()
      stkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND STK_SUPERBUILD_EP_VARS VTK_DIR:PATH)

  endif()

endif()

