#!
#! Stores all known plug-in targets (potentially also from external projects)
#! in the variable specified by the first argument.
#!
#! \param var_targets A variable name containing the output.
#! \ingroup CMakeUtilities
function(stkFunctionGetAllPluginTargets var_targets)

  if(NOT var_targets)
    message(FATAL_ERROR "Missing variable name in argument list")
  endif()

  set(plugin_targets )
  get_property(STK_PLUGIN_LIBRARIES_VARS GLOBAL PROPERTY STK_PLUGIN_LIBRARIES_VARS)
  if(STK_PLUGIN_LIBRARIES_VARS)
    list(REMOVE_DUPLICATES STK_PLUGIN_LIBRARIES_VARS)
    foreach(_var ${STK_PLUGIN_LIBRARIES_VARS})
      if(${_var})
        list(APPEND plugin_targets ${${_var}})
      endif()
    endforeach()
  endif()

  set(${var_targets} ${plugin_targets} PARENT_SCOPE)

endfunction()
