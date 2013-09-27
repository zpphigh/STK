###########################################################################
#
#  Library:   STK
#
#  Copyright (c) fitark.org
#
#	National Laboratory for Information Science and Technology, Tsinghua University
#	Department of Interventional Ultrasound, Chinese PLA General Hospital
#
###########################################################################

#
# Depends on:
#  STK/CMake/stkMacroParseArguments.cmake
#


#! When STK is built as shared library, the following macro builds a python module
#! associated with the generated PythonQt wrappers. When loaded, it will
#! dynamically loads both the (1) generated decorators and the (2) hand written one.
#! On the other hand, when STK is built statically, it creates a
#! static library providing a initialization function that will allow to load
#! both (1) and (2).

#! \ingroup CMakeAPI
macro(stkMacroBuildLibWrapper)
  stkMacroParseArguments(MY
    "NAMESPACE;TARGET;SRCS;WRAPPER_LIBRARY_TYPE;ARCHIVE_OUTPUT_DIRECTORY;LIBRARY_OUTPUT_DIRECTORY;RUNTIME_OUTPUT_DIRECTORY;INSTALL_BIN_DIR;INSTALL_LIB_DIR"
    "NO_INSTALL"
    ${ARGN}
    )

  # Sanity checks
  if(NOT DEFINED MY_TARGET)
    message(FATAL_ERROR "NAME is mandatory")
  endif()
  if(NOT DEFINED MY_WRAPPER_LIBRARY_TYPE OR "${MY_WRAPPER_LIBRARY_TYPE}" STREQUAL "SHARED")
    set(MY_WRAPPER_LIBRARY_TYPE "MODULE")
  endif()

  if(NOT DEFINED MY_NAMESPACE)
    set(MY_NAMESPACE "org.commontk")
  endif()
  if(NOT DEFINED STK_WRAP_PYTHONQT_FULL)
    set(STK_WRAP_PYTHONQT_FULL FALSE)
  endif()
  foreach(type RUNTIME LIBRARY ARCHIVE)
    if(NOT DEFINED MY_${type}_OUTPUT_DIRECTORY)
      set(MY_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
    endif()
  endforeach()
  if(NOT DEFINED MY_INSTALL_BIN_DIR)
    set(MY_INSTALL_BIN_DIR ${STK_INSTALL_BIN_DIR})
  endif()
  if(NOT DEFINED MY_INSTALL_LIB_DIR)
    set(MY_INSTALL_LIB_DIR ${STK_INSTALL_LIB_DIR})
  endif()

  # Define library name
  set(lib_name ${MY_TARGET})

  # Include dirs
  set(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  # Since the PythonQt decorator depends on PythonQt, Python and VTK, let's link against
  # these ones to avoid complaints of MSVC
  # Note: "LINK_DIRECTORIES" has to be invoked before "ADD_LIBRARY"
  set(my_EXTRA_PYTHON_LIBRARIES ${PYTHON_LIBRARY} ${PYTHONQT_LIBRARIES})

  # Does a header having the expected filename exists ?
  string(REGEX REPLACE "^STK" "stk" lib_name_lc_stk ${lib_name})
  set(DECORATOR_HEADER ${lib_name_lc_stk}PythonQtDecorators.h)
  set(HAS_DECORATOR FALSE)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DECORATOR_HEADER})
    set(HAS_DECORATOR TRUE)
    set(DECORATOR_HEADER ${DECORATOR_HEADER})
    set_source_files_properties(${DECORATOR_HEADER} WRAP_EXCLUDE)
  endif()
  #message("HAS_DECORATOR:${HAS_DECORATOR}")
  #message("path/to/DECORATOR_HEADER:${CMAKE_CURRENT_SOURCE_DIR}/${DECORATOR_HEADER}")

  set(KIT_PYTHONQT_SRCS) # Clear variable
  stkMacroWrapPythonQt(${MY_NAMESPACE} ${lib_name}
    KIT_PYTHONQT_SRCS "${MY_SRCS}" ${STK_WRAP_PYTHONQT_FULL} ${HAS_DECORATOR})
  if(HAS_DECORATOR)
    list(APPEND KIT_PYTHONQT_SRCS ${DECORATOR_HEADER})
    QT4_WRAP_CPP(KIT_PYTHONQT_SRCS ${DECORATOR_HEADER} OPTIONS -f${DECORATOR_HEADER})
  endif()
  add_library(${lib_name}PythonQt ${MY_WRAPPER_LIBRARY_TYPE} ${KIT_PYTHONQT_SRCS})
  target_link_libraries(${lib_name}PythonQt ${lib_name} ${my_EXTRA_PYTHON_LIBRARIES})
  if(MY_WRAPPER_LIBRARY_TYPE STREQUAL "STATIC")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set_target_properties(${lib_name}PythonQt PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
  endif()
  if(MY_WRAPPER_LIBRARY_TYPE STREQUAL "MODULE")
    # Make sure that no prefix is set on the library
    set_target_properties(${lib_name}PythonQt PROPERTIES PREFIX "")
    # Python extension modules on Windows must have the extension ".pyd"
    # instead of ".dll" as of Python 2.5.  Older python versions do support
    # this suffix.
    # See http://docs.python.org/faq/windows.html#is-a-pyd-file-the-same-as-a-dll
    if(WIN32 AND NOT CYGWIN)
      set_target_properties(${lib_name}PythonQt PROPERTIES SUFFIX ".pyd")
    endif()
  endif()
  set_target_properties(${lib_name}PythonQt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${MY_RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${MY_LIBRARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY "${MY_ARCHIVE_OUTPUT_DIRECTORY}"
    )

  # Set labels associated with the target.
  set_target_properties(${lib_name}PythonQt PROPERTIES LABELS ${lib_name})

  # Update list of libraries wrapped with PythonQt
  set(STK_WRAPPED_LIBRARIES_PYTHONQT
    ${STK_WRAPPED_LIBRARIES_PYTHONQT} ${lib_name}
    CACHE INTERNAL "STK libraries wrapped using PythonQt" FORCE)

  # Install rules
  if(NOT MY_NO_INSTALL AND MY_WRAPPER_LIBRARY_TYPE STREQUAL "MODULE")
    install(TARGETS ${lib_name}PythonQt
      RUNTIME DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
      LIBRARY DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
      ARCHIVE DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT Development)
  endif()

endmacro()


