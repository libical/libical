# CMake support macros and functions for the libical project

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

function(libical_option option description)
  set(extra_option_arguments ${ARGN})
  option(${option} "${description}" ${extra_option_arguments})
  add_feature_info("Option ${option}" ${option} "${description}")
endfunction()

function(libical_deprecated_option deprecated_option option description)
  set(extra_option_arguments ${ARGN})
  message(STATUS "WARNING: ${deprecated_option} is deprecated. Use ${option} instead")
  libical_option(${option} "${description}" ${extra_option_arguments})
endfunction()

function(libical_append_if condition value)
  if(${condition})
    foreach(variable ${ARGN})
      set(${variable} "${${variable}} ${value}" PARENT_SCOPE)
    endforeach()
  endif()
endfunction()

macro(libical_add_cflag flag name)
  check_c_compiler_flag("${flag}" "C_SUPPORTS_${name}")
  libical_append_if("C_SUPPORTS_${name}" "${flag}" CMAKE_C_FLAGS)
endmacro()

macro(libical_add_cxxflag flag name)
  check_cxx_compiler_flag("${flag}" "CXX_SUPPORTS_${name}")
  libical_append_if("CXX_SUPPORTS_${name}" "${flag}" CMAKE_CXX_FLAGS)
endmacro()
