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

#macro to build (but do not create a test)
macro(vcbuildme _name _srcs)
  add_executable(${_name} ${_srcs})
  add_dependencies(${_name} icalvcard)
  target_link_libraries(${_name} icalvcard)
endmacro()

# Build the test and add the test, making sure the test properties are set
macro(vctestme _name _srcs)
  vcbuildme(${_name} "${_srcs}")
  add_test(NAME ${_name} COMMAND ${_name})
  setprops(${_name})
endmacro()

vctestme(vcard_construct vcard_test.c)
vctestme(vcard_encode vcard_test_encode.c)

vcbuildme(vcardcomponent_fuzz1 vcardcomponent_fuzz1.c)
if(LIBICAL_BUILD_TESTING_BIGFUZZ)
  foreach(
    fuzztest
    479882034 # MSAN abrt
    479881583 # ASAN cpu timeout exceeds 60s
    480258833 # MSAN floating-point exception
    481458325 # MSAN Use-of-uninitialized value
    485387226 # Null-dereference READ
    # TODO: 485932113 # ASAN Heap-use-after-free
  )
    add_test(
      NAME vcfuzz1test${fuzztest}
      COMMAND
        vcardcomponent_fuzz1 "${PROJECT_SOURCE_DIR}/test-data/vcardfuzz${fuzztest}"
    )
    setprops(vcfuzz1test${fuzztest})
  endforeach()
endif()
