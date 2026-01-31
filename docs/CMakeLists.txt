# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

add_subdirectory(reference)

#
# API docs
#
macro(_join_list _var _sep)
  set(${_var} "")
  foreach(_element ${ARGN})
    set(${_var} "${${_var}}${_sep}${_element}")
  endforeach()
endmacro()

find_package(Doxygen)
set_package_properties(
  Doxygen
  PROPERTIES
    TYPE
      OPTIONAL
    DESCRIPTION
      "API Documentation system"
    URL
      "https://www.doxygen.org"
    PURPOSE
      "Needed to build the API documentation."
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps *.md)
  file(
    GLOB _all_hdrs
    ${PROJECT_SOURCE_DIR}/src/libical/*.h
    ${PROJECT_SOURCE_DIR}/src/libical/*.c
    ${PROJECT_SOURCE_DIR}/src/libical/*.cpp
    ${PROJECT_SOURCE_DIR}/src/libical/*.hpp
    ${PROJECT_BINARY_DIR}/src/libical/*.c
    ${PROJECT_BINARY_DIR}/src/libical/*.h
    ${PROJECT_SOURCE_DIR}/src/libicalss/*.h
    ${PROJECT_SOURCE_DIR}/src/libicalss/*.c
    ${PROJECT_SOURCE_DIR}/src/libicalss/*.cpp
    ${PROJECT_SOURCE_DIR}/src/libicalss/*.hpp
    ${PROJECT_BINARY_DIR}/src/libicalss/*.c
    ${PROJECT_BINARY_DIR}/src/libicalss/*.h
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*.h
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*.c
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*.cpp
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*.hpp
    ${PROJECT_BINARY_DIR}/src/libicalvcard/*.c
    ${PROJECT_BINARY_DIR}/src/libicalvcard/*.h
  )
  list(APPEND _all_hdrs ${_dox_deps})
  file(
    GLOB _no_hdrs
    ${PROJECT_SOURCE_DIR}/src/libical/*impl.h
    ${PROJECT_SOURCE_DIR}/src/libical/*_export.h
    ${PROJECT_SOURCE_DIR}/src/libical/libical_deprecated.h
    ${PROJECT_SOURCE_DIR}/src/libical/libical_sentinel.h
    ${PROJECT_SOURCE_DIR}/src/libical/icalpvl.c
    ${PROJECT_SOURCE_DIR}/src/libical/icalpvl.h
    ${PROJECT_SOURCE_DIR}/src/libical/*_p.c
    ${PROJECT_SOURCE_DIR}/src/libical/*_p.h
    ${PROJECT_BINARY_DIR}/src/libical/*_p.h
    ${PROJECT_BINARY_DIR}/src/libical/icalderived*.c
    ${PROJECT_BINARY_DIR}/src/libical/icalderived*.h
    ${PROJECT_SOURCE_DIR}/src/libicalss/icalssyacc.*
    ${PROJECT_SOURCE_DIR}/src/libicalss/*impl.h
    ${PROJECT_SOURCE_DIR}/src/libicalss/*_export.h
    ${PROJECT_SOURCE_DIR}/src/libicalss/*_p.c
    ${PROJECT_SOURCE_DIR}/src/libicalss/*_p.h
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*impl.h
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*_export.h
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*_p.c
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/*_p.h
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderived*.c
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderived*.h
  )
  list(REMOVE_ITEM _all_hdrs ${_no_hdrs})
  _join_list(DOXYGEN_INPUT " " ${_all_hdrs})

  #apidox generation using doxygen
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/apidocs/html)
  add_custom_command(
    OUTPUT
      ${DOXYGEN_OUTPUT_DIR}/index.html
    COMMAND
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMAND
      ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/src/libicalvcal
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/src/libicalvcal/README.txt
      ${DOXYGEN_OUTPUT_DIR}/src/libicalvcal
    DEPENDS
      ${_all_hdrs}
      ${_dox_deps}
      ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Run Doxygen"
  )
  add_custom_target(
    docs
    DEPENDS
      ${PROJECT_BINARY_DIR}/apidocs/html/index.html
    COMMENT "Target to build the documentation"
  )
  add_dependencies(
    docs
    ical
    icalss
    icalvcal
  )
  if(LIBICAL_CXX_BINDINGS)
    add_dependencies(
      docs
      ical_cxx
      icalss_cxx
    )
  endif()
endif()
