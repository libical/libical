# CMake support macros and functions for the libical project

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# Call option() and then add_feature_info()
function(libical_option option description)
  set(extra_option_arguments ${ARGN})
  option(${option} "${description}" ${extra_option_arguments})
  add_feature_info(
    "Option ${option}"
    ${option}
    "${description}"
  )
endfunction()

# Warn about deprecated cmake options then call libical_option
function(libical_deprecated_option deprecated_option option description)
  set(extra_option_arguments ${ARGN})
  if(${deprecated_option})
    message(WARNING "${deprecated_option} is deprecated. Use ${option} instead")
    set(${option} ${deprecated_option} CACHE BOOL "${description}")
  endif()
  libical_option(${option} "${description}" ${extra_option_arguments})
endfunction()

# If condition is True, append the specified value to each ARGN
function(libical_append_if condition value)
  if(${condition})
    foreach(variable ${ARGN})
      set(${variable} "${${variable}} ${value}" PARENT_SCOPE)
    endforeach()
  endif()
endfunction()

# Create a variable C_SUPPORTS_<flag> with a boolean denoting
# if the C compiler supports that flag; if so, append the flag
# to the global CMAKE_C_FLAGS variable.
macro(libical_add_cflag flag name)
  check_c_compiler_flag(
    "${flag}"
    "C_SUPPORTS_${name}"
  )
  libical_append_if("C_SUPPORTS_${name}" "${flag}" CMAKE_C_FLAGS)
endmacro()

# Create a variable CXX_SUPPORTS_<flag> with a boolean denoting
# if the C++ compiler supports that flag; if so, append the flag
# to the global CMAKE_CXX_FLAGS variable.
macro(libical_add_cxxflag flag name)
  check_cxx_compiler_flag(
    "${flag}"
    "CXX_SUPPORTS_${name}"
  )
  libical_append_if("CXX_SUPPORTS_${name}" "${flag}" CMAKE_CXX_FLAGS)
endmacro()
