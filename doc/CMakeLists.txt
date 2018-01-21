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
  PURPOSE "Needed to build the API documention."
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps *.dox *.html)
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

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/apidocs/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
  )
  add_dependencies(docs ical icalss icalvcal)
  if(WITH_CXX_BINDINGS)
    add_dependencies(docs ical_cxx icalss_cxx)
  endif()
endif()
