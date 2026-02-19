# SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

add_definitions(-Dlibical_vcard_EXPORTS)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libical
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

if(ICU_FOUND)
  include_directories(${ICU_INCLUDE_DIRS})
endif()

set(ICALSCRIPTS ${PROJECT_SOURCE_DIR}/scripts/)
set(BUILT_HEADERS) #list of derived headers we build
set(BUILD_SOURCES) #list of derived implementations we build

set(
  PROPERTYDEPS
  ${ICALSCRIPTS}/mkderivedproperties.pl
  ${PROJECT_SOURCE_DIR}/design-data/vcard-properties.csv
  ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv
)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.h.in -h
    ${PROJECT_SOURCE_DIR}/design-data/vcard-properties.csv ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv >
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h
  DEPENDS
    ${PROPERTYDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.h.in
  COMMENT "Generate vcardderivedproperty.h"
)
list(APPEND BUILT_HEADERS ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.c.in -c
    ${PROJECT_SOURCE_DIR}/design-data/vcard-properties.csv ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv >
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c
  DEPENDS
    ${PROPERTYDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.c.in
  COMMENT "Generate vcardderivedproperty.c"
)
list(APPEND BUILT_SOURCES ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c)

set(
  PARAMETERDEPS
  ${ICALSCRIPTS}/mkderivedparameters.pl
  ${PROJECT_SOURCE_DIR}/design-data/vcard-parameters.csv
)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.h.in -h
    ${PROJECT_SOURCE_DIR}/design-data/vcard-parameters.csv >
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h
  DEPENDS
    ${PARAMETERDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.h.in
  COMMENT "Generate vcardderivedparameter.h"
)
list(APPEND BUILT_HEADERS ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.c.in -c
    ${PROJECT_SOURCE_DIR}/design-data/vcard-parameters.csv >
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c
  DEPENDS
    ${PARAMETERDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.c.in
  COMMENT "Generate vcardderivedparameter.c"
)
list(APPEND BUILT_SOURCES ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c)

set(
  RESTRICTIONDEPS
  ${ICALSCRIPTS}/mkrestrictiontable.pl
  ${PROJECT_SOURCE_DIR}/design-data/vcard-restrictions.csv
)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardrestriction.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardrestriction.c.in
    ${PROJECT_SOURCE_DIR}/design-data/vcard-restrictions.csv > ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardrestriction.c
  DEPENDS
    ${RESTRICTIONDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardrestriction.c.in
  COMMENT "Generate vcardrestriction.c"
)
list(APPEND BUILT_SOURCES ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardrestriction.c)

set(
  VALUEDEPS
  ${ICALSCRIPTS}mkderivedvalues.pl
  ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv
)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.h.in -h
    ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv > ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h
  DEPENDS
    ${VALUEDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.h.in
  COMMENT "Generate vcardderivedvalue.h"
)
list(APPEND BUILT_HEADERS ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -v -i
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.c.in -c
    ${PROJECT_SOURCE_DIR}/design-data/vcard-value-types.csv > ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c
  DEPENDS
    ${VALUEDEPS}
    ${PROJECT_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.c.in
  COMMENT "Generate vcardderivedvalue.c"
)
list(APPEND BUILT_SOURCES ${PROJECT_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c)

if(WIN32)
  set(TOPS "\"${PROJECT_SOURCE_DIR}\"")
  set(TOPB "\"${PROJECT_BINARY_DIR}\"")
else()
  set(TOPS "${PROJECT_SOURCE_DIR}")
  set(TOPB "${PROJECT_BINARY_DIR}")
endif()

add_custom_target(
  icalvcard-header
  ALL
  DEPENDS
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcard.h
  COMMENT "Target to generate the aggregate vcard.h header"
)

set(
  icalvcard_LIB_SRCS
  ${BUILT_SOURCES}
  ${BUILT_HEADERS}
  libical_vcard_export.h
  vcardcomponent.c
  vcardcomponent.h
  vcardenumarray.h
  vcardparameter.c
  vcardparameter.h
  vcardparameterimpl.h
  vcardparser.c
  vcardparser.h
  vcardproperty_p.h
  vcardproperty.c
  vcardproperty.h
  vcardstrarray.h
  vcardstructured.c
  vcardstructured.h
  vcardtextlist.c
  vcardtextlist.h
  vcardtime.c
  vcardtime.h
  vcardtypes.h
  vcardvalue.c
  vcardvalue.h
  vcardvalueimpl.h
)

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcard.h
  COMMAND
    ${CMAKE_COMMAND} -DTOPS:FILEPATH=${TOPS} -DTOPB:FILEPATH=${TOPB}
    -DVCARD_FILE_H_FILE:FILEPATH=${PROJECT_BINARY_DIR}/src/libicalvcard/vcard.h -P
    ${CMAKE_CURRENT_SOURCE_DIR}/vcard_file.cmake
  DEPENDS
    ${icalvcard_LIB_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/vcard_file.cmake
  COMMENT "Generate the aggregate vcard.h header"
)

add_library(
  icalvcard
  ${LIBRARY_TYPE}
  ${icalvcard_LIB_SRCS}
)
add_dependencies(
  icalvcard
  ical-header
  icalvcard-header
)
target_link_libraries(icalvcard ical)

if(MSVC)
  set_target_properties(
    icalvcard
    PROPERTIES
      PREFIX
        "lib"
  )
endif()
if(NOT ANDROID)
  set_target_properties(
    icalvcard
    PROPERTIES
      VERSION
        ${CMAKE_PROJECT_VERSION}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()
set_target_properties(
  icalvcard
  PROPERTIES
    CLEAN_DIRECT_OUTPUT
      1
)

install(TARGETS icalvcard EXPORT icalTargets DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libicalvcard.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libicalvcard.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libicalvcard.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

########### install files ###############

install(
  FILES
    ${PROJECT_BINARY_DIR}/src/libicalvcard/vcard.h
    ${BUILT_HEADERS}
    libical_vcard_export.h
    vcardtime.h
    vcardtypes.h
    vcardcomponent.h
    vcardenumarray.h
    vcardparameter.h
    vcardparser.h
    vcardproperty.h
    vcardstrarray.h
    vcardstructured.h
    vcardvalue.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/libical
)
