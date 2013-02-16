include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libicalss ${CMAKE_BINARY_DIR}/src/libicalss
  ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libicalvcal ${CMAKE_BINARY_DIR}/src/libicalvcal
)

#these are generated sources, but we keep them in the repo
set(icalvcal_LIB_DEVSRCS vcc.c)

########### next target ###############

set(icalvcal_LIB_SRCS
  icalvcal.h
  icalvcal.c
  port.h
  vcc.h
  vobject.h
  vobject.c
  vcaltmp.h
  vcaltmp.c
  ${icalvcal_LIB_DEVSRCS}
)

if(MSVC)
  list(APPEND icalvcal_LIB_SRCS ../icalvcal.def)
endif(MSVC)

add_library(icalvcal ${LIBRARY_TYPE} ${icalvcal_LIB_SRCS})
add_library(icalvcal-static STATIC ${icalvcal_LIB_SRCS})

add_dependencies(icalvcal ical-header)
add_dependencies(icalvcal-static ical-header)

target_link_libraries(icalvcal ical)

if(MSVC)
  set_target_properties(icalvcal PROPERTIES OUTPUT_NAME "libicalvcal")
  set_target_properties(icalvcal-static PROPERTIES OUTPUT_NAME "libicalvcal-static")
else(MSVC)
  set_target_properties(icalvcal-static PROPERTIES OUTPUT_NAME "icalvcal")
endif(MSVC)
set_target_properties(icalvcal PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(icalvcal PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(icalvcal-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS icalvcal icalvcal-static ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
  icalvcal.h port.h vcc.h vobject.h vcaltmp.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
