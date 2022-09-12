add_definitions(-Dlibicalvcard_EXPORTS)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libical
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

if(ICU_FOUND)
  include_directories(${ICU_INCLUDE_DIRS})
endif()

set(ICALSCRIPTS ${CMAKE_SOURCE_DIR}/scripts/)
set(BUILT_HEADERS) #list of derived headers we build
set(BUILD_SOURCES) #list of derived implementations we build

set(PROPERTYDEPS
  ${ICALSCRIPTS}/mkderivedproperties.pl
  ${CMAKE_SOURCE_DIR}/design-data/vcard-properties.csv
  ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.h.in
      -h ${CMAKE_SOURCE_DIR}/design-data/vcard-properties.csv
      ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h
  DEPENDS
    ${PROPERTYDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.h.in
  COMMENT "Generate vcardderivedproperty.h"
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/vcard-properties.csv
      ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c
  DEPENDS
    ${PROPERTYDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedproperty.c.in
  COMMENT "Generate vcardderivedproperty.c"
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedproperty.c)

set(PARAMETERDEPS
  ${ICALSCRIPTS}/mkderivedparameters.pl
  ${CMAKE_SOURCE_DIR}/design-data/vcard-parameters.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.h.in
      -h ${CMAKE_SOURCE_DIR}/design-data/vcard-parameters.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h
  DEPENDS
    ${PARAMETERDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.h.in
  COMMENT "Generate vcardderivedparameter.h"
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/vcard-parameters.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c
  DEPENDS
    ${PARAMETERDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedparameter.c.in
  COMMENT "Generate vcardderivedparameter.c"
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedparameter.c)

set(RESTRICTIONDEPS
  ${ICALSCRIPTS}/mkrestrictiontable.pl
  ${CMAKE_SOURCE_DIR}/design-data/vcard-restrictions.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardrestriction.c
  COMMAND
    ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardrestriction.c.in
      ${CMAKE_SOURCE_DIR}/design-data/vcard-restrictions.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardrestriction.c
  DEPENDS
    ${RESTRICTIONDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardrestriction.c.in
  COMMENT "Generate vcardrestriction.c"
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardrestriction.c)

set(VALUEDEPS
  ${ICALSCRIPTS}mkderivedvalues.pl
  ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.h.in
      -h ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h
  DEPENDS
    ${VALUEDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.h.in
  COMMENT "Generate vcardderivedvalue.h"
)
list(APPEND BUILT_HEADERS ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.h)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c
  COMMAND
    ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -v
      -i ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.c.in
      -c ${CMAKE_SOURCE_DIR}/design-data/vcard-value-types.csv >
      ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c
  DEPENDS
    ${VALUEDEPS}
    ${CMAKE_SOURCE_DIR}/src/libicalvcard/vcardderivedvalue.c.in
  COMMENT "Generate vcardderivedvalue.c"
)
list(APPEND BUILT_SOURCES ${CMAKE_BINARY_DIR}/src/libicalvcard/vcardderivedvalue.c)

if(WIN32)
  set(TOPS "\"${CMAKE_SOURCE_DIR}\"")
  set(TOPB "\"${CMAKE_BINARY_DIR}\"")
else()
  set(TOPS "${CMAKE_SOURCE_DIR}")
  set(TOPB "${CMAKE_BINARY_DIR}")
endif()

add_custom_target(
  vcard-header ALL
  DEPENDS ${CMAKE_BINARY_DIR}/src/libicalvcard/vcard.h
  COMMENT "Target to generate the aggregate vcard.h header"
)

set(icalvcard_LIB_SRCS
  ${BUILT_SOURCES}
  ${BUILT_HEADERS}
  libical_vcard_export.h
  vcardcomponent.c
  vcardcomponent.h
  vcardenumarray.c
  vcardenumarray.h
  vcardparameter.c
  vcardparameter.h
  vcardparameterimpl.h
  vcardparser.c
  vcardparser.h
  vcardproperty.c
  vcardproperty.h
  vcardstrarray.c
  vcardstrarray.h
  vcardtime.c
  vcardtime.h
  vcardvalue.c
  vcardvalue.h
  vcardvalueimpl.h
)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/src/libicalvcard/vcard.h
  COMMAND
    ${CMAKE_COMMAND}
      -DTOPS:FILEPATH=${TOPS}
      -DTOPB:FILEPATH=${TOPB}
      -DVCARD_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libicalvcard/vcard.h
      -P ${CMAKE_CURRENT_SOURCE_DIR}/vcard_file.cmake
  DEPENDS ${icalvcard_LIB_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/vcard_file.cmake
  COMMENT "Generate the aggregate vcard.h header"
)

add_library(icalvcard ${LIBRARY_TYPE} ${icalvcard_LIB_SRCS})
add_dependencies(ical ical-header)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  add_library(icalvcard-static STATIC ${icalvcard_LIB_SRCS})
  add_dependencies(icalvcard-static ical-header)
elseif(STATIC_ONLY)
  add_library(icalvcard-static ALIAS icalvcard)
endif()

if(ICU_FOUND)
  target_link_libraries(icalvcard ${ICU_LIBRARIES})
endif()

if ( MSVC )
    target_compile_options(icalvcard PRIVATE /W4 /WX)
else()
  target_compile_options(icalvcard PRIVATE -Wall -Wextra -Werror)
endif()

if(MSVC)
  set_target_properties(icalvcard PROPERTIES PREFIX "lib")
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(icalvcard-static PROPERTIES PREFIX "lib")
  endif()
else()
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(icalvcard-static PROPERTIES OUTPUT_NAME "icalvcard")
  endif()
endif()
set_target_properties(icalvcard PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(icalvcard PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  set_target_properties(icalvcard-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

install(
  TARGETS icalvcard
  EXPORT icalTargets
  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  install(
    TARGETS icalvcard-static
    EXPORT icalTargets
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
endif()

########### install files ###############

install(FILES
  ${CMAKE_BINARY_DIR}/src/libicalvcard/vcard.h
  ${BUILT_HEADERS}
  libical_vcard_export.h
  vcardtime.h
  vcardcomponent.h
  vcardenumarray.h
  vcardparameter.h
  vcardparser.h
  vcardproperty.h
  vcardstrarray.h
  vcardvalue.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
