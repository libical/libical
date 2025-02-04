# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

add_definitions(-Dlibical_icalss_EXPORTS)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libicalss.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libicalss.pc @ONLY)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libical
)
if(BDB_FOUND)
  include_directories(${BDB_INCLUDE_DIR})
endif()

if(WIN32)
  set(TOPS "\"${PROJECT_SOURCE_DIR}\"")
  set(TOPB "\"${PROJECT_BINARY_DIR}\"")
else()
  set(TOPS "${PROJECT_SOURCE_DIR}")
  set(TOPB "${PROJECT_BINARY_DIR}")
endif()

add_custom_target(
  icalss-header
  ALL
  DEPENDS
    ${PROJECT_BINARY_DIR}/src/libicalss/icalss.h
  COMMENT "Target to generate the aggregate libicalss header"
)

########### next target ###############

#these are generated sources, but we keep them in the repo
set(
  icalss_LIB_DEVSRCS
  icalsslexer.c
  icalssyacc.c
)

set(
  icalss_LIB_SRCS
  libical_icalss_export.h
  icalcalendar.c
  icalcalendar.h
  icalclassify.c
  icalclassify.h
  icalcluster.c
  icalcluster.h
  icalclusterimpl.h
  icalgauge.c
  icalgauge.h
  icalgaugeimpl.h
  icaldirset.c
  icaldirset.h
  icaldirsetimpl.h
  icalfileset.c
  icalfileset.h
  icalfilesetimpl.h
  icalset.c
  icalset.h
  icalssyacc.h
  icalspanlist.c
  icalspanlist.h
  icalmessage.c
  icalmessage.h
  ${icalss_LIB_DEVSRCS}
)
if(BDB_FOUND)
  list(
    APPEND
    icalss_LIB_SRCS
    icalbdbset.c
    icalbdbset.h
    icalbdbsetimpl.h
  )
endif()

add_custom_command(
  OUTPUT
    ${PROJECT_BINARY_DIR}/src/libicalss/icalss.h
  COMMAND
    ${CMAKE_COMMAND} -DTOPS:FILEPATH=${TOPS} -DTOPB:FILEPATH=${TOPB}
    -DICAL_FILE_H_FILE:FILEPATH=${PROJECT_BINARY_DIR}/src/libicalss/icalss.h -DBDB_FOUND=${BDB_FOUND} -P
    ${CMAKE_CURRENT_SOURCE_DIR}/icalss_file.cmake
  DEPENDS
    ${icalss_LIB_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/icalss_file.cmake
  COMMENT "Generate the aggregate libicalss header"
)

add_library(
  icalss
  ${LIBRARY_TYPE}
  ${icalss_LIB_SRCS}
)
add_dependencies(
  icalss
  ical-header
  icalss-header
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  add_library(icalss-static STATIC ${icalss_LIB_SRCS})
  add_dependencies(
    icalss-static
    ical-header
    icalss-header
  )
elseif(STATIC_ONLY)
  add_library(icalss-static ALIAS icalss)
endif()

target_link_libraries(icalss ical)
if(BDB_FOUND)
  target_link_libraries(icalss ${BDB_LIBRARY})
endif()

if(MSVC)
  set_target_properties(
    icalss
    PROPERTIES
      PREFIX
        "lib"
  )
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(
      icalss-static
      PROPERTIES
        PREFIX
          "lib"
    )
  endif()
else()
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(
      icalss-static
      PROPERTIES
        OUTPUT_NAME
          "icalss"
    )
  endif()
endif()
if(NOT ANDROID)
  set_target_properties(
    icalss
    PROPERTIES
      VERSION
        ${LIBICAL_LIB_VERSION_STRING}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()
set_target_properties(
  icalss
  PROPERTIES
    CLEAN_DIRECT_OUTPUT
      1
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  set_target_properties(
    icalss-static
    PROPERTIES
      CLEAN_DIRECT_OUTPUT
        1
  )
endif()

install(
  TARGETS
    icalss
  EXPORT icalTargets
  DESTINATION
  ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
  install(
    TARGETS
      icalss-static
    EXPORT icalTargets
    DESTINATION
    ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
endif()

########### next target ###############

if(WITH_CXX_BINDINGS)
  set(
    icalsscxx_LIB_SRCS
    icalspanlist_cxx.cpp
    icalspanlist_cxx.h
  )
  if(BDB_FOUND)
    list(APPEND icalsscxx_LIB_SRCS icalbdbset_cxx.h)
  endif()
  add_library(
    icalss_cxx
    ${LIBRARY_TYPE}
    ${icalsscxx_LIB_SRCS}
  )
  add_dependencies(
    icalss_cxx
    ical-header
    icalss-header
  )
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    add_library(icalss_cxx-static STATIC ${icalsscxx_LIB_SRCS})
    add_dependencies(
      icalss_cxx-static
      ical-header
      icalss-header
    )
  elseif(STATIC_ONLY)
    add_library(icalss_cxx-static ALIAS icalss_cxx)
  endif()

  target_link_libraries(
    icalss_cxx
    icalss
    ical_cxx
  )
  if(DEFINED CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(icalss_cxx ${CMAKE_THREAD_LIBS_INIT})
  endif()

  if(MSVC)
    set_target_properties(
      icalss_cxx
      PROPERTIES
        PREFIX
          "lib"
    )
    if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
      set_target_properties(
        icalss_cxx-static
        PROPERTIES
          PREFIX
            "lib"
      )
    endif()
  else()
    if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
      set_target_properties(
        icalss_cxx-static
        PROPERTIES
          OUTPUT_NAME
            "icalss_cxx"
      )
    endif()
  endif()
  if(NOT ANDROID)
    set_target_properties(
      icalss_cxx
      PROPERTIES
        VERSION
          ${LIBICAL_LIB_VERSION_STRING}
        SOVERSION
          ${LIBICAL_LIB_SOVERSION_STRING}
    )
  endif()
  set_target_properties(
    icalss_cxx
    PROPERTIES
      CLEAN_DIRECT_OUTPUT
        1
  )
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    set_target_properties(
      icalss_cxx-static
      PROPERTIES
        CLEAN_DIRECT_OUTPUT
          1
    )
  endif()

  install(
    TARGETS
      icalss_cxx
    EXPORT icalTargets
    DESTINATION
    ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
  if(NOT SHARED_ONLY AND NOT STATIC_ONLY)
    install(
      TARGETS
        icalss_cxx-static
      EXPORT icalTargets
      DESTINATION
      ${INSTALL_TARGETS_DEFAULT_ARGS}
    )
  endif()
endif()

########### install files ###############

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libicalss.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

install(
  FILES
    ${PROJECT_BINARY_DIR}/src/libicalss/icalss.h
    icalcalendar.h
    icalclassify.h
    icalcluster.h
    icaldirset.h
    icaldirsetimpl.h
    icalfileset.h
    icalfilesetimpl.h
    icalgauge.h
    icalgaugeimpl.h
    icalmessage.h
    icalset.h
    icalspanlist.h
    icalssyacc.h
    libical_icalss_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/libical
)

if(WITH_CXX_BINDINGS)
  install(FILES icalspanlist_cxx.h DESTINATION ${INCLUDE_INSTALL_DIR}/libical)
  if(BDB_FOUND)
    install(FILES icalbdbset_cxx.h DESTINATION ${INCLUDE_INSTALL_DIR}/libical)
  endif()
endif()
