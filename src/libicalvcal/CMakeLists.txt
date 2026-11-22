# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

if(CMAKE_C_COMPILER_IS_GCC OR CMAKE_C_COMPILER_IS_CLANG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion -Wno-sign-conversion")
endif()

set(
  include_directories
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libicalvcal
)

list(APPEND add_definitions ${LIBICAL_C_DEFINITIONS})

#these are generated sources, but we keep them in the repo
set(icalvcal_LIB_DEVSRCS vcc.c)

########### next target ###############

list(
  APPEND icalvcal_LIB_SRCS
  icalvcal.h
  icalvcal.c
  vcc.h
  vobject.h
  vobject.c
  vcaltmp.h
  vcaltmp.c
  ${icalvcal_LIB_DEVSRCS}
)

add_library(
  icalvcal
  ${LIBRARY_TYPE}
  ${icalvcal_LIB_SRCS}
)
add_dependencies(icalvcal ical-header)

generate_export_header(icalvcal BASE_NAME libical_vcal)
target_sources(icalvcal PRIVATE ${PROJECT_BINARY_DIR}/src/libicalvcal/libical_vcal_export.h)

target_compile_definitions(icalvcal PRIVATE ${add_definitions})
target_include_directories(icalvcal PRIVATE ${include_directories})

target_link_libraries(icalvcal ical)

if(MSVC)
  set_target_properties(
    icalvcal
    PROPERTIES
      PREFIX
        "lib"
  )
endif()
if(NOT ANDROID)
  set_target_properties(
    icalvcal
    PROPERTIES
      VERSION
        ${libical_VERSION}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()
set_target_properties(
  icalvcal
  PROPERTIES
    CLEAN_DIRECT_OUTPUT
      1
)

install(TARGETS icalvcal EXPORT icalTargets DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libicalvcal.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libicalvcal.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libicalvcal.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

########### install files ###############

install(
  FILES
    icalvcal.h
    vcaltmp.h
    vcc.h
    vobject.h
    ${PROJECT_BINARY_DIR}/src/libicalvcal/libical_vcal_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/libical
)
