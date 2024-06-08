# SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

include(CTest)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/libicalvcard
  ${PROJECT_BINARY_DIR}/src/libicalvcard
)

# Build the test and add the test, making sure the test properties are set
macro(vctestme _name _srcs)
  add_executable(${_name} ${_srcs})
  add_dependencies(${_name} icalvcard)
  target_link_libraries(${_name} icalvcard)
  add_test(NAME ${_name} COMMAND ${_name})
  setprops(${_name})
endmacro()

vctestme(vcard_construct vcard_test.c)
vctestme(vcard_encode vcard_test_encode.c)
