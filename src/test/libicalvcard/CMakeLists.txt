include(CTest)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libical
  ${CMAKE_BINARY_DIR}/src/libical
  ${CMAKE_SOURCE_DIR}/src/libicalvcard
  ${CMAKE_BINARY_DIR}/src/libicalvcard
)

add_executable(vcard_test vcard_test.c)
add_dependencies(vcard_test ical icalvcard)
target_link_libraries(vcard_test ical icalvcard)

add_test(NAME vcard_hello_world
  COMMAND vcard_test ${CMAKE_SOURCE_DIR}/test-data/test.vcf
)
