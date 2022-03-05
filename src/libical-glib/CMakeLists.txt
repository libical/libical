add_definitions(-Dlibical_ical_EXPORTS)

# a C11 compliant compiler is required to build this library
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# build ical-glib-src-generator
add_executable(ical-glib-src-generator
  tools/generator.c
  tools/generator.h
  tools/xml-parser.c
  tools/xml-parser.h
)

target_compile_options(ical-glib-src-generator PUBLIC ${GLIB_CFLAGS} ${LIBXML_CFLAGS} -DG_LOG_DOMAIN=\"src-generator\")
target_link_libraries(ical-glib-src-generator ${GLIB_LIBRARIES} ${LIBXML_LIBRARIES})

install(
  TARGETS ical-glib-src-generator
  EXPORT IcalGlibSrcGenerator
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libical
)
install(
  EXPORT IcalGlibSrcGenerator
  NAMESPACE native-
  DESTINATION ${LIB_INSTALL_DIR}/cmake/LibIcal
)

list(APPEND API_FILES
  api/i-cal-array.xml
  api/i-cal-attach.xml
  api/i-cal-comp-iter.xml
  api/i-cal-component.xml
  api/i-cal-datetimeperiod.xml
  api/i-cal-derived-parameter.xml
  api/i-cal-derived-property.xml
  api/i-cal-derived-value.xml
  api/i-cal-duration.xml
  api/i-cal-enums.xml
  api/i-cal-error.xml
  api/i-cal-geo.xml
  api/i-cal-memory.xml
  api/i-cal-mime.xml
  api/i-cal-parameter.xml
  api/i-cal-parser.xml
  api/i-cal-period.xml
  api/i-cal-property.xml
  api/i-cal-recur-iterator.xml
  api/i-cal-recurrence.xml
  api/i-cal-recur.xml
  api/i-cal-reqstat.xml
  api/i-cal-restriction.xml
  api/i-cal-time-span.xml
  api/i-cal-time.xml
  api/i-cal-timezone.xml
  api/i-cal-trigger.xml
  api/i-cal-unknowntokenhandling.xml
  api/i-cal-value.xml
)

list(APPEND LIBICAL_GLIB_HEADERS
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

  list(APPEND LIBICAL_GLIB_SOURCES ${h_file} ${c_file})
  list(APPEND LIBICAL_GLIB_HEADERS ${h_file})
  list(APPEND xml_files ${xml_file_fullpath})
endforeach()

if(CMAKE_CROSSCOMPILING)
  # import native ical-glib-src-generator when cross-compiling
  set(IMPORT_ICAL_GLIB_SRC_GENERATOR "ICAL_GLIB_SRC_GENERATOR-NOTFOUND"
    CACHE FILEPATH
    "Path to exported ical-glib-src-generator target from native build"
  )
  include(${IMPORT_ICAL_GLIB_SRC_GENERATOR})
  set(ical-glib-src-generator_EXE native-ical-glib-src-generator)
else()
  set(ical-glib-src-generator_EXE ical-glib-src-generator)
endif()

add_custom_command (
  OUTPUT ${LIBICAL_GLIB_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/libical-glib-private.h ${CMAKE_CURRENT_BINARY_DIR}/i-cal-forward-declarations.h
  COMMAND ${ical-glib-src-generator_EXE} "${CMAKE_CURRENT_SOURCE_DIR}/tools" "${CMAKE_CURRENT_SOURCE_DIR}/api"
  DEPENDS ${ical-glib-src-generator_EXE} ${xml_files}
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/i-cal-object.c.in
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.c
  COPYONLY
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/i-cal-object.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.h
  COPYONLY
)

list(APPEND LIBICAL_GLIB_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/libical-glib-private.h
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.c
  ${CMAKE_CURRENT_BINARY_DIR}/i-cal-object.h
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libical
)

add_library(ical-glib ${LIBRARY_TYPE} ${LIBICAL_GLIB_SOURCES})
add_dependencies(ical-glib ical-header)
target_compile_options(ical-glib PRIVATE ${GLIB_CFLAGS})
target_compile_definitions(ical-glib PRIVATE -DG_LOG_DOMAIN="libical-glib" -DLIBICAL_GLIB_COMPILATION)
target_link_libraries(ical-glib PRIVATE ical ${GLIB_LIBRARIES})
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  add_library(ical-glib-static STATIC ${LIBICAL_GLIB_SOURCES})
  add_dependencies(ical-glib-static ical-header)
  target_compile_options(ical-glib-static PUBLIC ${GLIB_CFLAGS} -DG_LOG_DOMAIN="libical-glib" -DLIBICAL_GLIB_COMPILATION)
  target_link_libraries(ical-glib-static ${GLIB_LIBRARIES})
endif()

# GObject Introspection
if(HAVE_INTROSPECTION)
  # extract only file names without path for introspection
  foreach(file IN LISTS LIBICAL_GLIB_SOURCES)
    string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/" "" plain_file ${file})
    list(APPEND LIBICAL_GLIB_GIR_SOURCES ${plain_file})
  endforeach()

  include(GObjectIntrospectionMacros)
  set(LIBICAL_GLIB_GIR_VERSION_STRING "${LIBICAL_LIB_MAJOR_VERSION}.0")
  set(LIB_VERSION "${LIBICAL_LIB_MAJOR_VERSION}_0")

  set(GObjectIntrospection_GIRS)
  set(GObjectIntrospection_SCANNER_FLAGS)
  set(GObjectIntrospection_SCANNER_ARGS)
  set(GObjectIntrospection_COMPILER_ARGS)

  set(GObjectIntrospection_SCANNER_ARGS
    --add-include-path=${CMAKE_CURRENT_BINARY_DIR}
    --add-include-path=${CMAKE_CURRENT_SOURCE_DIR}
    --add-include-path=${CMAKE_BINARY_DIR}/src
    --add-include-path=${CMAKE_BINARY_DIR}/src/libical
    --add-include-path=${CMAKE_SOURCE_DIR}/src
    --add-include-path=${CMAKE_SOURCE_DIR}/src/libical
    --library-path=${LIBRARY_OUTPUT_PATH}
    --identifier-prefix=ICal
    --pkg-export libical-glib
    --c-include=libical-glib/libical-glib.h
    --cflags-begin
    -I${CMAKE_BINARY_DIR}/src
    -I${CMAKE_BINARY_DIR}/src/libical
    -I${CMAKE_BINARY_DIR}/src/libical-glib
    -I${LIBRARY_OUTPUT_PATH}
    -I${CMAKE_SOURCE_DIR}/src
    -I${CMAKE_SOURCE_DIR}/src/libical
    --cflags-end
    --verbose
  )

  set(ICalGLib_${LIB_VERSION}_gir "ICalGLib")
  set(ICalGLib_${LIB_VERSION}_gir_SCANNERFLAGS "--warn-all")
  set(ICalGLib_${LIB_VERSION}_gir_VERSION ${LIBICAL_GLIB_GIR_VERSION_STRING})
  set(ICalGLib_${LIB_VERSION}_gir_LIBRARY "ICalGLib")
  set(ICalGLib_${LIB_VERSION}_gir_INCLUDES GObject-2.0 GLib-2.0)
  set(ICalGLib_${LIB_VERSION}_gir_CFLAGS ${GLIB_CFLAGS} -DLIBICAL_GLIB_COMPILATION -I${CMAKE_CURRENT_BINARY_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_BINARY_DIR}/src/libical -I${CMAKE_SOURCE_DIR}/src/libical -I${CMAKE_BINARY_DIR}/src/libical-glib)
  set(ICalGLib_${LIB_VERSION}_gir_LIBS ical-glib)
  set(ICalGLib_${LIB_VERSION}_gir_FILES ${LIBICAL_GLIB_GIR_SOURCES})

  list(APPEND GObjectIntrospection_GIRS ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.gir)

  gir_add_introspections(GObjectIntrospection_GIRS)
endif()

if(ICAL_GLIB_VAPI)
  add_custom_target(vala ALL)
  set(gir_fullname ${CMAKE_BINARY_DIR}/src/libical-glib/ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.gir)
  set(metadata_fullname ${CMAKE_CURRENT_BINARY_DIR}/ICalGLib-${LIBICAL_GLIB_GIR_VERSION_STRING}.metadata)
  set(vapi_file ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.vapi)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ICalGLib.metadata.in
    ${metadata_fullname}
    @ONLY
  )

  add_custom_command(OUTPUT ${vapi_file}
    COMMAND ${VAPIGEN}
      --vapidir=${CMAKE_CURRENT_SOURCE_DIR}
      --vapidir=${CMAKE_CURRENT_BINARY_DIR}
      --girdir=${CMAKE_BINARY_DIR}/src/libical-glib
      --pkg gio-2.0
      --library libical-glib
      --metadatadir=${CMAKE_CURRENT_SOURCE_DIR}
      --metadatadir=${CMAKE_CURRENT_BINARY_DIR}
      ${gir_fullname}
    DEPENDS
      ${gir_fullname}
      ${metadata_fullname}
  )

  add_custom_target(valafile DEPENDS ${vapi_file})

  add_dependencies(vala valafile)

  install(FILES ${vapi_file} DESTINATION ${SHARE_INSTALL_DIR}/vala/vapi)
endif()

if(MSVC)
  set_target_properties(ical-glib PROPERTIES PREFIX "lib")
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(ical-glib-static PROPERTIES PREFIX "lib")
  endif()
else()
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(ical-glib-static PROPERTIES OUTPUT_NAME "ical-glib")
  endif()
endif()
set_target_properties(ical-glib PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(ical-glib PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  set_target_properties(ical-glib-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

install(
  TARGETS ical-glib
  EXPORT icalTargets
  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  install(
    TARGETS ical-glib-static
    EXPORT icalTargets
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
endif()

########### install files ###############

install(FILES ${LIBICAL_GLIB_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/libical-glib
)

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

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/libical-glib.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.pc
  @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
