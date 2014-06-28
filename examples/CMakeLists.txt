include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libicalss ${CMAKE_BINARY_DIR}/src/libicalss
)

########### next target ###############

set(doesnothing_SRCS
  access_components.c
  access_properties_and_parameters.c
  errors.c
  main.c
  parse_text.c
)

add_executable(doesnothing ${doesnothing_SRCS})

target_link_libraries(doesnothing ical icalss icalvcal ical icalss icalvcal)

########### install files ###############

