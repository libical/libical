include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libicalss ${CMAKE_BINARY_DIR}/src/libicalss
  ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_BINARY_DIR}/src/libical
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
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
  COMMAND
  ${CMAKE_COMMAND}
    -DTOPS:FILEPATH=${TOPS}
    -DTOPB:FILEPATH=${TOPB}
    -DICAL_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
    -P ${CMAKE_CURRENT_SOURCE_DIR}/icalss_file.cmake
  DEPENDS
  ical-header
)

add_custom_target(icalss-header
  DEPENDS
  ical-header
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
)

########### next target ###############

#these are generated sources, but we keep them in the repo
set(icalss_LIB_DEVSRCS icalsslexer.c icalssyacc.c)

set(icalss_LIB_SRCS
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

if(MSVC)
  list(APPEND icalss_LIB_SRCS ../icalss.def)
endif(MSVC)

add_library(icalss ${LIBRARY_TYPE} ${icalss_LIB_SRCS})
add_library(icalss-static STATIC ${icalss_LIB_SRCS})

add_dependencies(icalss icalss-header)
add_dependencies(icalss-static icalss-header)

target_link_libraries(icalss ical)

if(MSVC)
  set_target_properties(icalss PROPERTIES OUTPUT_NAME "libicalss")
  set_target_properties(icalss-static PROPERTIES OUTPUT_NAME "libicalss-static")
else(MSVC)
  set_target_properties(icalss-static PROPERTIES OUTPUT_NAME "icalss")
endif(MSVC)
set_target_properties(icalss PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(icalss PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(icalss-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS icalss icalss-static ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
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
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
