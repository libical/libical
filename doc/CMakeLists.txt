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
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "API Documentation system"
  URL "http://www.doxygen.org"
  PURPOSE "Needed to build the API documentation."
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps *.dox *.html *.md ${CMAKE_SOURCE_DIR}/README.md)
  file(GLOB _all_hdrs
    ${CMAKE_SOURCE_DIR}/src/libical/*.h
    ${CMAKE_SOURCE_DIR}/src/libical/*.c
    ${CMAKE_SOURCE_DIR}/src/libical/*.cxx
    ${CMAKE_BINARY_DIR}/src/libical/*.c
    ${CMAKE_BINARY_DIR}/src/libical/*.h

    ${CMAKE_SOURCE_DIR}/src/libicalss/*.h
    ${CMAKE_SOURCE_DIR}/src/libicalss/*.c
    ${CMAKE_SOURCE_DIR}/src/libicalss/*.cxx
    ${CMAKE_BINARY_DIR}/src/libicalss/*.c
    ${CMAKE_BINARY_DIR}/src/libicalss/*.h

    ${CMAKE_SOURCE_DIR}/src/libicalvcal/*.h
    ${CMAKE_SOURCE_DIR}/src/libicalvcal/*.c
    ${CMAKE_SOURCE_DIR}/src/libicalvcal/*.cxx
    ${CMAKE_BINARY_DIR}/src/libicalvcal/*.c
    ${CMAKE_BINARY_DIR}/src/libicalvcal/*.h
  )
  list(APPEND _all_hdrs ${_dox_deps})
  _join_list(DOXYGEN_INPUT " " ${_all_hdrs})

  #apidox generation using doxygen
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )

  set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/apidocs/html)
  add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT_DIR}/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    #copy some files by-hand that are referred to by the markdown README
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Install.txt ${DOXYGEN_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/src/libicalvcal
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/src/libicalvcal/README.txt ${DOXYGEN_OUTPUT_DIR}/src/libicalvcal
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Run Doxygen"
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
    COMMENT "Target to build the documentation"
  )
  add_dependencies(docs ical icalss icalvcal)
  if(WITH_CXX_BINDINGS)
    add_dependencies(docs ical_cxx icalss_cxx)
  endif()
endif()
