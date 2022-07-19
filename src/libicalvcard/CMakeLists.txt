add_definitions(-Dlibicalvcard_EXPORTS)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libical
)

if(ICU_FOUND)
  include_directories(${ICU_INCLUDE_DIRS})
endif()

set(icalvcard_LIB_SRCS
  vcard.c
  vcard.h
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
  vcard.h
  libicalvcard_export.h
  DESTINATION
  ${INCLUDE_INSTALL_DIR}/libical
)
