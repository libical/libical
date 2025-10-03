# SPDX-FileCopyrightText: Milan Crha <mcrha@redhat.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

if(CMAKE_C_COMPILER_IS_GCC OR CMAKE_C_COMPILER_IS_CLANG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion -Wno-sign-conversion")
endif()

add_definitions(-Dlibical_ical_EXPORTS)

# a C11 compliant compiler is required to build this library
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# build ical-glib-src-generator
add_executable(
  ical-glib-src-generator
  tools/generator.c
  tools/generator.h
  tools/header-parser.c
  tools/header-parser.h
  tools/xml-parser.c
  tools/xml-parser.h
)

target_compile_options(
  ical-glib-src-generator
  PUBLIC
    ${GLIB_CFLAGS}
    ${LIBXML_CFLAGS}
    -DG_LOG_DOMAIN=\"src-generator\"
)
target_link_libraries(
  ical-glib-src-generator
  ${GLIB_LIBRARIES}
  ${LIBXML_LIBRARIES}
)

install(
  TARGETS
    ical-glib-src-generator
  EXPORT IcalGlibSrcGenerator
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libical
)
install(EXPORT IcalGlibSrcGenerator NAMESPACE native- DESTINATION ${LIB_INSTALL_DIR}/cmake/LibIcal)

# iCalendar files
list(
  APPEND
  API_FILES
  api/i-cal-array.xml
  api/i-cal-attach.xml
  api/i-cal-comp-iter.xml
  api/i-cal-component.xml
  api/i-cal-datetimeperiod.xml
  api/i-cal-derived-parameter.xml
  api/i-cal-derived-property.xml
  api/i-cal-derived-value.xml
  api/i-cal-duration.xml
  api/i-cal-enum-array.xml
  api/i-cal-enums.xml
  api/i-cal-error.xml
  api/i-cal-geo.xml
  api/i-cal-memory.xml
  api/i-cal-parameter.xml
  api/i-cal-param-iter.xml
  api/i-cal-parser.xml
  api/i-cal-period.xml
  api/i-cal-property.xml
  api/i-cal-prop-iter.xml
  api/i-cal-recur-iterator.xml
  api/i-cal-recurrence.xml
  api/i-cal-recur.xml
  api/i-cal-reqstat.xml
  api/i-cal-restriction.xml
  api/i-cal-str-array.xml
  api/i-cal-time-span.xml
  api/i-cal-time.xml
  api/i-cal-timezone.xml
  api/i-cal-trigger.xml
  api/i-cal-unknowntokenhandling.xml
  api/i-cal-value.xml
)

# vCard files
list(
  APPEND
  API_FILES
  api/i-cal-vcard-component.xml
  api/i-cal-vcard-derived-parameter.xml
  api/i-cal-vcard-derived-property.xml
  api/i-cal-vcard-derived-value.xml
  api/i-cal-vcard-enum-array.xml
  api/i-cal-vcard-geo.xml
  api/i-cal-vcard-parameter.xml
  api/i-cal-vcard-parser.xml
  api/i-cal-vcard-property.xml
  api/i-cal-vcard-restriction.xml
  api/i-cal-vcard-str-array.xml
  api/i-cal-vcard-structured.xml
  api/i-cal-vcard-time.xml
  api/i-cal-vcard-tz.xml
  api/i-cal-vcard-value.xml
)

list(
  APPEND
  LIBICAL_GLIB_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.h
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.h
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-forward-declarations.h
)

# add the command to generate the source code from the api files
foreach(file IN LISTS API_FILES)
  string(REPLACE "api/" "${CMAKE_CURRENT_SOURCE_DIR}/api/" xml_file_fullpath ${file})
  string(REPLACE "api/" "${CMAKE_CURRENT_BINARY_DIR}/" bin_file ${file})
  string(REGEX REPLACE ".xml$" ".h" h_file ${bin_file})
  string(REGEX REPLACE ".xml$" ".c" c_file ${bin_file})

  list(
    APPEND
    LIBICAL_GLIB_SOURCES
    ${h_file}
    ${c_file}
  )
  list(APPEND LIBICAL_GLIB_HEADERS ${h_file})
  list(APPEND xml_files ${xml_file_fullpath})
endforeach()

if(CMAKE_CROSSCOMPILING)
  # import native ical-glib-src-generator when cross-compiling
  set(
    IMPORT_ICAL_GLIB_SRC_GENERATOR
    "ICAL_GLIB_SRC_GENERATOR-NOTFOUND"
    CACHE FILEPATH
    "Path to exported ical-glib-src-generator target from native build"
  )
  include(${IMPORT_ICAL_GLIB_SRC_GENERATOR})
  set(ical-glib-src-generator_EXE native-ical-glib-src-generator)
else()
  set(ical-glib-src-generator_EXE ical-glib-src-generator)
endif()

list(
  APPEND
  LIBICAL_GLIB_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/libical-glib-private.h
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.c
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.h
)

add_custom_command(
  OUTPUT
    ${LIBICAL_GLIB_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/i-cal-forward-declarations.h
    ${CMAKE_CURRENT_BINARY_DIR}/ical-glib-build-check.c
  COMMAND
    ${ical-glib-src-generator_EXE} "${CMAKE_CURRENT_SOURCE_DIR}/tools" "${CMAKE_CURRENT_SOURCE_DIR}/api"
  DEPENDS
    ${ical-glib-src-generator_EXE}
    ${xml_files}
  COMMENT "Generate the libical-glib sources"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/i-cal-object.c.in ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.c COPYONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/i-cal-object.h.in ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.h COPYONLY)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libicalvcard
  ${PROJECT_SOURCE_DIR}/src/libicalvcard
)

add_library(
  ical-glib
  ${LIBRARY_TYPE}
  ${LIBICAL_GLIB_SOURCES}
)
add_dependencies(
  ical-glib
  ical-header
  icalvcard-header
)
target_compile_options(ical-glib PRIVATE ${GLIB_CFLAGS})
target_compile_definitions(
  ical-glib
  PRIVATE
    -DG_LOG_DOMAIN="libical-glib"
    -DLIBICAL_GLIB_COMPILATION
)
target_link_libraries(
  ical-glib
  PRIVATE
    ical
    icalvcard
    ${GLIB_LIBRARIES}
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  add_library(ical-glib-static STATIC ${LIBICAL_GLIB_SOURCES})
  add_dependencies(ical-glib-static ical-header)
  target_compile_options(
    ical-glib-static
    PUBLIC
      ${GLIB_CFLAGS}
      -DG_LOG_DOMAIN="libical-glib"
      -DLIBICAL_GLIB_COMPILATION
  )
  target_link_libraries(ical-glib-static ${GLIB_LIBRARIES})
endif()

if(LIBICAL_GLIB_CHECK_API_FILES)
  add_custom_command(
    TARGET ical-glib
    POST_BUILD
    COMMAND
      ${ical-glib-src-generator_EXE} "${CMAKE_CURRENT_SOURCE_DIR}/tools" "${CMAKE_CURRENT_SOURCE_DIR}/api"
      "--check-api-files" "${CMAKE_CURRENT_BINARY_DIR}/../libical/ical.h" "LIBICAL_ICAL_EXPORT"
      "${CMAKE_CURRENT_BINARY_DIR}/../libicalvcard/vcard.h" "LIBICAL_VCARD_EXPORT"
    COMMENT "Check the libical-glib API files for completeness"
  )
endif()

include(CheckCCompilerFlag)

check_c_compiler_flag(
  -Wswitch-enum
  c_flag_Wswitch_enum_supported
)
check_c_compiler_flag(
  -Wswitch
  c_flag_Wswitch_supported
)
if(c_flag_Wswitch_enum_supported AND c_flag_Wswitch_supported)
  add_executable(ical-glib-build-check ${CMAKE_CURRENT_BINARY_DIR}/ical-glib-build-check.c)
  add_dependencies(ical-glib-build-check ical-glib)
  target_link_libraries(
    ical-glib-build-check
    PRIVATE
      ical-glib
      ${GLIB_LIBRARIES}
  )
  target_compile_options(
    ical-glib-build-check
    PRIVATE
      ${GLIB_CFLAGS}
      -DLIBICAL_GLIB_UNSTABLE_API=1
      -DLIBICAL_GLIB_COMPILATION
      -Wswitch-enum
      -Wswitch
  )
else()
  message(NOTICE "compiler does not support both -Wswitch-enum and -Wswitch, skipping libical-glib-build-check")
endif()

# GObject Introspection
if(LIBICAL_GOBJECT_INTROSPECTION)
  # extract only file names without path for introspection
  foreach(file IN LISTS LIBICAL_GLIB_SOURCES)
    string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/" "" plain_file ${file})
    list(APPEND LIBICAL_GLIB_GIR_SOURCES ${plain_file})
  endforeach()

  include(GObjectIntrospectionMacros)
  set(LIBICAL_GLIB_GIR_VERSION_STRING "${LIBICAL_LIB_MAJOR_SOVERSION}.0")
  set(LIB_VERSION "${LIBICAL_LIB_MAJOR_SOVERSION}_0")

  set(GObjectIntrospection_GIRS)
  set(GObjectIntrospection_SCANNER_FLAGS)
  set(GObjectIntrospection_SCANNER_ARGS)
  set(GObjectIntrospection_COMPILER_ARGS)

  set(
    GObjectIntrospection_SCANNER_ARGS
    --add-include-path=${CMAKE_CURRENT_BINARY_DIR}
    --add-include-path=${CMAKE_CURRENT_SOURCE_DIR}
    --add-include-path=${PROJECT_BINARY_DIR}/src
    --add-include-path=${PROJECT_BINARY_DIR}/src/libical
    --add-include-path=${PROJECT_BINARY_DIR}/src/libicalvcard
    --add-include-path=${PROJECT_SOURCE_DIR}/src
    --add-include-path=${PROJECT_SOURCE_DIR}/src/libical
    --add-include-path=${PROJECT_SOURCE_DIR}/src/libicalvcard
    --library-path=${LIBRARY_OUTPUT_PATH}
    --identifier-prefix=ICal
    --pkg-export
    libical-glib
    --c-include=libical-glib/libical-glib.h
    --cflags-begin
    -I${PROJECT_BINARY_DIR}/src
    -I${PROJECT_BINARY_DIR}/src/libical
    -I${PROJECT_BINARY_DIR}/src/libicalvcard
    -I${PROJECT_BINARY_DIR}/src/libical-glib
    -I${LIBRARY_OUTPUT_PATH}
    -I${PROJECT_SOURCE_DIR}/src
    -I${PROJECT_SOURCE_DIR}/src/libical
    -I${PROJECT_SOURCE_DIR}/src/libicalvcard
    --cflags-end
    --verbose
  )

  set(ICalGLib_${LIB_VERSION}_gir "ICalGLib")
  set(ICalGLib_${LIB_VERSION}_gir_SCANNERFLAGS "--warn-all")
  set(ICalGLib_${LIB_VERSION}_gir_VERSION ${LIBICAL_GLIB_GIR_VERSION_STRING})
  set(ICalGLib_${LIB_VERSION}_gir_LIBRARY "ICalGLib")
  set(
    ICalGLib_${LIB_VERSION}_gir_INCLUDES
    GObject-2.0
    GLib-2.0
  )
  set(
    ICalGLib_${LIB_VERSION}_gir_CFLAGS
    ${GLIB_CFLAGS}
    -DLIBICAL_GLIB_COMPILATION
    -I${CMAKE_CURRENT_BINARY_DIR}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -I${PROJECT_BINARY_DIR}/src/libical
    -I${PROJECT_SOURCE_DIR}/src/libical
    -I${PROJECT_BINARY_DIR}/src/libicalvcard
    -I${PROJECT_SOURCE_DIR}/src/libicalvcard
    -I${PROJECT_BINARY_DIR}/src/libical-glib
  )
  set(ICalGLib_${LIB_VERSION}_gir_LIBS ical-glib)
  set(ICalGLib_${LIB_VERSION}_gir_FILES ${LIBICAL_GLIB_GIR_SOURCES})

  list(APPEND GObjectIntrospection_GIRS ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.gir)

  gir_add_introspections(GObjectIntrospection_GIRS)
endif()

if(LIBICAL_GLIB_VAPI)
  add_custom_target(vala ALL COMMENT "Target to run vala")
  set(gir_fullname ${PROJECT_BINARY_DIR}/src/libical-glib/ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.gir)
  set(typelib_fullname ${PROJECT_BINARY_DIR}/src/libical-glib/ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.typelib)
  set(metadata_fullname ${CMAKE_CURRENT_BINARY_DIR}/ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.metadata)
  set(vapi_file ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.vapi)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ICalGLib.metadata.in ${metadata_fullname} @ONLY)

  add_custom_command(
    OUTPUT
      ${vapi_file}
    COMMAND
      ${VAPIGEN} --vapidir=${CMAKE_CURRENT_SOURCE_DIR} --vapidir=${CMAKE_CURRENT_BINARY_DIR}
      --girdir=${PROJECT_BINARY_DIR}/src/libical-glib --pkg gio-2.0 --library libical-glib
      --metadatadir=${CMAKE_CURRENT_SOURCE_DIR} --metadatadir=${CMAKE_CURRENT_BINARY_DIR} ${gir_fullname}
    DEPENDS
      ical-glib
      ${gir_fullname}
      ${LIBICAL_GLIB_GIR_SOURCES}
      ${GObjectIntrospection_GIRS}
      ${typelib_fullname}
      ${metadata_fullname}
    COMMENT "Run the tool to generate the Vala API"
  )

  add_custom_target(valafile DEPENDS ${vapi_file} COMMENT "Target to run valafile")

  add_dependencies(vala valafile)

  install(FILES ${vapi_file} DESTINATION ${SHARE_INSTALL_DIR}/vala/vapi)
endif()

if(MSVC)
  set_target_properties(
    ical-glib
    PROPERTIES
      PREFIX
        "lib"
  )
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(
      ical-glib-static
      PROPERTIES
        PREFIX
          "lib"
    )
  endif()
else()
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(
      ical-glib-static
      PROPERTIES
        OUTPUT_NAME
          "ical-glib"
    )
  endif()
endif()
if(NOT ANDROID)
  set_target_properties(
    ical-glib
    PROPERTIES
      VERSION
        ${CMAKE_PROJECT_VERSION}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()
set_target_properties(
  ical-glib
  PROPERTIES
    CLEAN_DIRECT_OUTPUT
      1
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  set_target_properties(
    ical-glib-static
    PROPERTIES
      CLEAN_DIRECT_OUTPUT
        1
  )
endif()

install(TARGETS ical-glib EXPORT icalTargets DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  install(TARGETS ical-glib-static EXPORT icalTargets DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()

########### install files ###############

install(FILES ${LIBICAL_GLIB_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/libical-glib)

########### create and install pkg-config file #########

set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
if(IS_ABSOLUTE ${LIB_INSTALL_DIR})
  set(libdir "${LIB_INSTALL_DIR}")
else()
  set(libdir "\${exec_prefix}/${LIB_INSTALL_DIR}")
endif()
if(IS_ABSOLUTE ${INCLUDE_INSTALL_DIR})
  set(includedir "${INCLUDE_INSTALL_DIR}")
else()
  set(includedir "\${prefix}/include")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libical-glib.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
