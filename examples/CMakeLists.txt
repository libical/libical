# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libicalss
)

########### doesnothing target ###############

set(doesnothing_SRCS
  access_components.c
  access_properties_and_parameters.c
  errors.c
  main.c
  parse_text.c
)

add_executable(doesnothing ${doesnothing_SRCS})
add_dependencies(doesnothing ical icalss icalvcal)

if(NOT STATIC_ONLY)
  target_link_libraries(doesnothing ical icalss icalvcal)
else()
  target_link_libraries(doesnothing
    ical-static
    icalss-static
    icalvcal-static
  )
  if(DEFINED CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(doesnothing ${CMAKE_THREAD_LIBS_INIT})
  endif()
  if(ICU_FOUND)
    target_link_libraries(doesnothing ${ICU_LIBRARIES})
  endif()
  if(BDB_FOUND)
    target_link_libraries(doesnothing ${BDB_LIBRARY})
  endif()
endif()
