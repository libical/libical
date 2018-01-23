add_definitions(-Dlibical_ical_EXPORTS)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/icalversion.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/icalversion.h
  @ONLY
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

if(ICU_FOUND)
  include_directories(${ICU_INCLUDE_DIR})
endif()

set(PACKAGE_DATA_DIR "\\\"${CMAKE_INSTALL_PREFIX}/share/libical\\\"")
add_definitions(-DPACKAGE_DATA_DIR="${PACKAGE_DATA_DIR}")

set(ICALSCRIPTS ${CMAKE_SOURCE_DIR}/scripts/)
set(BUILT_HEADERS) #list of derived headers we build
set(BUILD_SOURCES) #list of derived implementations we build

set(PROPERTYDEPS
  ${ICALSCRIPTS}/mkderivedproperties.pl
  ${CMAKE_SOURCE_DIR}/design-data/properties.csv
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
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
  DEPENDS
    ${PROPERTYDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/properties.csv
      ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
  DEPENDS
    ${PROPERTYDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c)

set(PARAMETERDEPS
  ${ICALSCRIPTS}/mkderivedparameters.pl
  ${CMAKE_SOURCE_DIR}/design-data/parameters.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
      -h ${CMAKE_SOURCE_DIR}/design-data/parameters.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  DEPENDS
    ${PARAMETERDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/parameters.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  DEPENDS
    ${PARAMETERDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c)

set(RESTRICTIONDEPS
  ${ICALSCRIPTS}/mkrestrictiontable.pl
  ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
      ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  DEPENDS
    ${RESTRICTIONDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c)

set(VALUEDEPS
  ${ICALSCRIPTS}mkderivedvalues.pl
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
      -h ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  DEPENDS
    ${VALUEDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl
      -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  DEPENDS
    ${VALUEDEPS}
    ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c)

if(WIN32)
  set(TOPS "\"${CMAKE_SOURCE_DIR}\"")
  set(TOPB "\"${CMAKE_BINARY_DIR}\"")
else()
  set(TOPS "${CMAKE_SOURCE_DIR}")
  set(TOPB "${CMAKE_BINARY_DIR}")
endif()

add_custom_target(
  ical-header ALL
  DEPENDS ${CMAKE_BINARY_DIR}/src/libical/ical.h
)

# GObject Introspection
if(HAVE_INTROSPECTION)
  include(GObjectIntrospectionMacros)
  set(GObjectIntrospection_GIRS)
  #set(GObjectIntrospection_SCANNER_ARGS "--add-include-path=${CMAKE_CURRENT_SOURCE_DIR}")
  #set(GObjectIntrospection_COMPILER_ARGS "--includedir=${CMAKE_CURRENT_SOURCE_DIR}")

  set(LIBICAL_GIR_VERSION_STRING "${LIBICAL_LIB_MAJOR_VERSION}.0")
  set(LIB_VERSION "${LIBICAL_LIB_MAJOR_VERSION}_0")
  set(ICal_${LIB_VERSION}_gir "libical")
  set(ICal_${LIB_VERSION}_gir_VERSION ${LIBICAL_GIR_VERSION_STRING})
  set(GObjectIntrospection_SCANNER_ARGS
    --c-include=libical/ical.h
    --pkg-export libical
    --identifier-prefix=ical
    -I${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/src/libical/ical.h
  )
  set(ICal_${LIB_VERSION}_gir_LIBRARY "ical")
  set(ICal_${LIB_VERSION}_gir_INCLUDES GObject-2.0)
  set(_includes ${_includes} "-L${LIBRARY_OUTPUT_PATH}")
  set(ICal_${LIB_VERSION}_gir_CFLAGS ${_includes})
  set(ICal_${LIB_VERSION}_gir_LIBS ical)

  list(APPEND GObjectIntrospection_GIRS ICal-${LIBICAL_GIR_VERSION_STRING}.gir)

  gir_add_introspections(GObjectIntrospection_GIRS)
endif()

########### next target ###############

set(ical_LIB_SRCS
  ${BUILT_SOURCES}
  ${BUILT_HEADERS}
  libical_ical_export.h
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
  icalerror.h
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
  icaltimezoneimpl.h
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
  qsort_gen.c 
  qsort_gen.h 
  icallangbind.h
  icallangbind.c
  caldate.c
  astime.h
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libical/ical.h
  COMMAND
    ${CMAKE_COMMAND}
      -DTOPS:FILEPATH=${TOPS}
      -DTOPB:FILEPATH=${TOPB}
      -DICAL_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libical/ical.h
      -P ${CMAKE_CURRENT_SOURCE_DIR}/ical_file.cmake
  DEPENDS ${ical_LIB_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/ical_file.cmake
)

add_library(ical ${LIBRARY_TYPE} ${ical_LIB_SRCS})
add_dependencies(ical ical-header)
if(NOT SHARED_ONLY)
  add_library(ical-static STATIC ${ical_LIB_SRCS})
  add_dependencies(ical-static ical-header)
endif()

target_link_libraries(ical ${CMAKE_THREAD_LIBS_INIT})

if(ICU_FOUND)
  target_link_libraries(ical ${ICU_LIBRARY})
endif()
if(ICU_I18N_FOUND)
  target_link_libraries(ical ${ICU_I18N_LIBRARY})
endif()

if(WINCE)
  target_link_libraries(ical ${WCECOMPAT_LIBRARIES})
endif()

if(MSVC)
  set_target_properties(ical PROPERTIES PREFIX "lib")
  if(NOT SHARED_ONLY)
    set_target_properties(ical-static PROPERTIES PREFIX "lib")
  endif()
else()
  if(NOT SHARED_ONLY)
    set_target_properties(ical-static PROPERTIES OUTPUT_NAME "ical")
  endif()
endif()
set_target_properties(ical PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(ical PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if(NOT SHARED_ONLY)
  set_target_properties(ical-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

install(
  TARGETS ical
  EXPORT icalTargets
  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(NOT SHARED_ONLY)
  install(
    TARGETS ical-static
    EXPORT icalTargets
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
endif()

########### next target ###############

if(WITH_CXX_BINDINGS)
  set(icalcxx_LIB_SRCS
    icalparameter_cxx.cpp
    icalparameter_cxx.h
    icalproperty_cxx.cpp
    icalproperty_cxx.h
    icalvalue_cxx.cpp
    icalvalue_cxx.h
    icptrholder_cxx.h
    vcomponent_cxx.cpp
    vcomponent_cxx.h
  )

  add_definitions(-DBUILD_LIBICALDLL)

  add_library(ical_cxx ${LIBRARY_TYPE} ${icalcxx_LIB_SRCS})
  add_dependencies(ical_cxx ical-header)
  if(NOT SHARED_ONLY)
    add_library(ical_cxx-static STATIC ${icalcxx_LIB_SRCS})
    add_dependencies(ical_cxx-static ical-header)
  endif()
  target_link_libraries(ical_cxx ical ${CMAKE_THREAD_LIBS_INIT})

  if(MSVC)
    set_target_properties(ical_cxx PROPERTIES PREFIX "lib")
    if(NOT SHARED_ONLY)
      set_target_properties(ical_cxx-static PROPERTIES PREFIX "lib")
    endif()
  else()
    if(NOT SHARED_ONLY)
      set_target_properties(ical_cxx-static PROPERTIES OUTPUT_NAME "ical_cxx")
    endif()
  endif()

  set_target_properties(ical_cxx PROPERTIES
    VERSION ${LIBICAL_LIB_VERSION_STRING}
    SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
  )
  set_target_properties(ical_cxx PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  if(NOT SHARED_ONLY)
    set_target_properties(ical_cxx-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  endif()

  install(
    TARGETS ical_cxx
    EXPORT icalTargets
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
  if(NOT SHARED_ONLY)
    install(
      TARGETS ical_cxx-static
      EXPORT icalTargets
      DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
    )
  endif()

  if(MSVC)
    remove_definitions(-DBUILD_LIBICALDLL)
  endif()

endif()

########### install files ###############

install(FILES
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
  icalarray.h
  icalattach.h
  icalcomponent.h
  ${BUILT_HEADERS}
  icalduration.h
  icalenums.h
  icalerror.h
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
  libical_ical_export.h
  pvl.h
  sspm.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)

if(WITH_CXX_BINDINGS)
  install(FILES
    icalparameter_cxx.h
    icalproperty_cxx.h
    icalvalue_cxx.h
    icptrholder_cxx.h
    vcomponent_cxx.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/libical
  )
endif()
