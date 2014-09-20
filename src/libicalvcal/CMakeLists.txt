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
endif()

add_library(icalvcal ${LIBRARY_TYPE} ${icalvcal_LIB_SRCS})
if(NOT SHARED_ONLY)
  add_library(icalvcal-static STATIC ${icalvcal_LIB_SRCS})
endif()

add_dependencies(icalvcal ical-header)
if(NOT SHARED_ONLY)
  add_dependencies(icalvcal-static ical-header)
endif()

target_link_libraries(icalvcal ical)

if(MSVC)
  set_target_properties(icalvcal PROPERTIES OUTPUT_NAME "libicalvcal")
  if(NOT SHARED_ONLY)
    set_target_properties(icalvcal-static PROPERTIES OUTPUT_NAME "libicalvcal-static")
  endif()
else()
  if(NOT SHARED_ONLY)
    set_target_properties(icalvcal-static PROPERTIES OUTPUT_NAME "icalvcal")
  endif()
endif()
set_target_properties(icalvcal PROPERTIES
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  SOVERSION ${LIBICAL_LIB_MAJOR_VERSION}
)
set_target_properties(icalvcal PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if(NOT SHARED_ONLY)
  set_target_properties(icalvcal-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

install(
  TARGETS icalvcal
  EXPORT icalTargets
  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(NOT SHARED_ONLY)
  install(
    TARGETS icalvcal-static
    EXPORT icalTargets
    DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
  )
endif()

########### install files ###############

install(FILES
  icalvcal.h port.h vcc.h vobject.h vcaltmp.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
