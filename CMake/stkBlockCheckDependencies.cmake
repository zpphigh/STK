
#
# Independently of the value of STK_SUPERBUILD, each external project definition will
# provides either the include and library directories or a variable name
# used by the corresponding Find<package>.cmake files.
#
# Within top-level CMakeLists.txt file, the variable names will be expanded if not in
# superbuild mode. The include and library dirs are then used in
# stkMacroBuildApp, stkMacroBuildLib, and stkMacroBuildPlugin
#

#-----------------------------------------------------------------------------
# Collect STK library target dependencies
#

stkMacroCollectAllTargetLibraries("${STK_LIBS}" "Libs" ALL_TARGET_LIBRARIES)
stkMacroCollectAllTargetLibraries("${STK_PLUGINS}" "Plugins" ALL_TARGET_LIBRARIES)
stkMacroCollectAllTargetLibraries("${STK_APPS}" "Applications" ALL_TARGET_LIBRARIES)
#message(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

#-----------------------------------------------------------------------------
# Initialize NON_STK_DEPENDENCIES variable
#
# Using the variable ALL_TARGET_LIBRARIES initialized above with the help
# of the macro stkMacroCollectAllTargetLibraries, let's get the list of all Non-STK dependencies.
# NON_STK_DEPENDENCIES is expected by the macro stkMacroShouldAddExternalProject
stkMacroGetAllNonProjectTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_STK_DEPENDENCIES)
#message(NON_STK_DEPENDENCIES:${NON_STK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
if(STK_SUPERBUILD)
  include(ExternalProject)
  include(stkMacroEmptyExternalProject)

  #set(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
  #set_property(DIRECTORY PROPERTY EP_BASE ${ep_base})

  set(ep_install_dir ${CMAKE_BINARY_DIR}/CMakeExternals/Install)
  set(ep_suffix      "-cmake")

  set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
  set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

  set(ep_common_cache_args
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
      -DBUILD_TESTING:BOOL=OFF
     )

  # Set CMake OSX variable to pass down the external projects
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
         -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
         -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
         -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
        )
    list(APPEND ep_common_cache_args ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS})
  endif()

  # Compute -G arg for configuring external projects with the same CMake generator:
  if(CMAKE_EXTRA_GENERATOR)
    set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
  else()
    set(gen "${CMAKE_GENERATOR}")
  endif()

  # Use this value where semi-colons are needed in ep_add args:
  set(sep "^^")

  # This variable will contain the list of CMake variable specific to each external project
  # that should passed to STK.
  # The item of this list should have the following form: <EP_VAR>:<TYPE>
  # where '<EP_VAR>' is an external project variable and TYPE is either BOOL, PATH or FILEPATH.
  # Variable appended to this list will be automatically exported in STKConfig.cmake, prefix 'STK_'
  # will be prepended if it applied.
  set(STK_SUPERBUILD_EP_VARS)
endif()

if(NOT DEFINED STK_POSSIBLE_DEPENDENCIES)
  message(FATAL_ERROR "error: STK_POSSIBLE_DEPENDENCIES variable is not defined !")
endif()
foreach(p ${STK_POSSIBLE_DEPENDENCIES})
  if(${p}_FILEPATH)
    include(${${p}_FILEPATH})
  else()
    include(CMakeExternals/${p}.cmake)
  endif()
endforeach()

#message("STK_POSSIBLE_DEPENDENCIES:")
#foreach(dep ${STK_POSSIBLE_DEPENDENCIES})
#  message("  ${dep}")
#endforeach()
