configure_file(${CMAKE_CURRENT_SOURCE_DIR}/icalversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/icalversion.h @ONLY)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libical
  ${CMAKE_BINARY_DIR}/src/libical
)

if(WIN32)
  if(MSVC)
    add_definitions(-DBUILD_LIBICALDLL)
  endif(MSVC)
endif(WIN32)

configure_file(icalerror.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/icalerror.h)

set(PACKAGE_DATA_DIR "\\\"${CMAKE_INSTALL_PREFIX}/share/libical\\\"")
add_definitions(-DPACKAGE_DATA_DIR="${PACKAGE_DATA_DIR}")

set(ICALSCRIPTS ${CMAKE_SOURCE_DIR}/scripts/)

set(PROPERTYDEPS
  ${ICALSCRIPTS}/mkderivedproperties.pl
  ${CMAKE_SOURCE_DIR}/design-data/properties.csv
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/properties.csv
    ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  DEPENDS ${PROPERTYDEPS}
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
  COMMAND
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/properties.csv
    ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
    DEPENDS ${PROPERTYDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
)

set(PARAMETERDEPS
  ${ICALSCRIPTS}/mkderivedparameters.pl
  ${CMAKE_SOURCE_DIR}/design-data/parameters.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/parameters.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  DEPENDS ${PARAMETERDEPS}
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/parameters.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  DEPENDS ${PARAMETERDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
)

set(RESTRICTIONDEPS
  ${ICALSCRIPTS}/mkrestrictiontable.pl
  ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src/libical/)
add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
    ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  DEPENDS ${RESTRICTIONDEPS}
)

set(VALUEDEPS
  ${ICALSCRIPTS}mkderivedvalues.pl
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  COMMAND
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  DEPENDS ${VALUEDEPS}
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  COMMAND
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  DEPENDS ${VALUEDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

if(WIN32)
  set(TOPS "\"${CMAKE_SOURCE_DIR}\"")
  set(TOPB "\"${CMAKE_BINARY_DIR}\"")
else(WIN32)
  set(TOPS "${CMAKE_SOURCE_DIR}")
  set(TOPB "${CMAKE_BINARY_DIR}")
endif(WIN32)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
  COMMAND
  ${CMAKE_COMMAND}
    -DTOPS:FILEPATH=${TOPS}
    -DTOPB:FILEPATH=${TOPB}
    -DICAL_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libical/ical.h
    -P ${CMAKE_CURRENT_SOURCE_DIR}/ical_file.cmake
  DEPENDS
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

add_custom_target(ical-header DEPENDS
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
)

macro(_list_prefix _outvar _listvar _prefix)
  set(${_outvar})
  foreach(_item IN LISTS ${_listvar})
    list(APPEND ${_outvar} ${_prefix}${_item})
  endforeach()
endmacro(_list_prefix)

# GObject Introspection
if(HAVE_INTROSPECTION)
  include(GObjectIntrospectionMacros)
  set(INTROSPECTION_GIRS)
  #set(INTROSPECTION_SCANNER_ARGS "--add-include-path=${CMAKE_CURRENT_SOURCE_DIR}")
  #set(INTROSPECTION_COMPILER_ARGS "--includedir=${CMAKE_CURRENT_SOURCE_DIR}")

  set(libical_0_48_gir "libical")
  set(libical_0_48_gir_VERSION "0.48")
  set(INTROSPECTION_SCANNER_ARGS ${CMAKE_BINARY_DIR}/src/libical/ical.h)
  set(libical_0_48_gir_LIBRARY "ical")
  set(libical_0_48_gir_INCLUDES GObject-2.0)
  get_directory_property(_tmp_includes ical_LIB_SRCS)
  _list_prefix(_includes _tmp_includes "-L")
  set(libical_0_48_gir_CFLAGS ${_includes})
  set(libical_0_48_gir_LIBS ical)

  list(APPEND INTROSPECTION_GIRS libical-0.48.gir)

  gir_add_introspections(INTROSPECTION_GIRS)
endif()

########### next target ###############

set(BUILT_COMBINEDHEADERS
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

set(BUILT_SOURCES
  ${BUILT_COMBINEDHEADERS}
  icalderivedparameter.c
  icalderivedproperty.c
  icalrestriction.c
  icalderivedvalue.c
)

set(ical_LIB_SRCS
  ${BUILT_SOURCES}
  icalarray.c
  icalarray.h
  icalattach.h
  icalattachimpl.h
  icalattach.c
  icalcomponent.c
  icalcomponent.h
  icalenums.c
  icalenums.h
  icalerror.c
  icalmemory.c
  icalmemory.h
  icalmime.c
  icalmime.h
  icalparameter.c
  icalparameter.h
  icalparameterimpl.h
  icalparser.c
  icalparser.h
  icalproperty.c
  icalproperty.h
  icalrecur.c
  icalrecur.h
  icalrestriction.h
  icaltime.c
  icaltime.h
  icaltz-util.c
  icaltz-util.h
  icaltimezone.c
  icaltimezone.h
  icalduration.h
  icalduration.c
  icalperiod.h
  icalperiod.c
  icaltypes.c
  icaltypes.h
  icalvalue.c
  icalvalue.h
  icalvalueimpl.h
  pvl.c
  pvl.h
  sspm.c
  sspm.h
  icallangbind.h
  icallangbind.c
  caldate.c
  astime.h
)
if(WIN32)
  if(MSVC)
    list(APPEND ical_LIB_SRCS ../ical.def)
  endif(MSVC)
endif(WIN32)

add_library(ical ${LIBRARY_TYPE} ${ical_LIB_SRCS})
add_library(ical-static STATIC ${ical_LIB_SRCS})

add_dependencies(ical ical-header)

target_link_libraries(ical ${CMAKE_THREAD_LIBS_INIT})

if(WINCE)
  target_link_libraries(ical ${WCECOMPAT_LIBRARIES})
endif(WINCE)

if(MSVC)
  set_target_properties(ical PROPERTIES OUTPUT_NAME "libical")
  set_target_properties(ical-static PROPERTIES OUTPUT_NAME "libical-static")
else(MSVC)
  set_target_properties(ical-static PROPERTIES OUTPUT_NAME "ical")
endif(MSVC)
set_target_properties(ical PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(ical PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ical-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS ical ical-static ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES ${CMAKE_SOURCE_DIR}/src/ical.h DESTINATION ${INCLUDE_INSTALL_DIR})

install(FILES
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
  icalarray.h
  icalattach.h
  icalcomponent.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  icalduration.h
  icalenums.h
  ${CMAKE_BINARY_DIR}/src/libical/icalerror.h
  icallangbind.h
  icalmemory.h
  icalmime.h
  icalparameter.h
  icalparser.h
  icalperiod.h
  icalproperty.h
  icalrecur.h
  icalrestriction.h
  icaltime.h
  icaltz-util.h
  icaltimezone.h
  icaltypes.h
  icalvalue.h
  pvl.h
  sspm.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
