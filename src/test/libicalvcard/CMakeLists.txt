#
# FILE: CMakeLists.txt
#
# CREATOR: Robert Stepanek 24 Aug 2022 <rsto@fastmailteam.com>
#
# SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#

include(CTest)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libicalvcard
  ${PROJECT_BINARY_DIR}/src/libicalvcard
)

add_executable(vcard_test vcard_test.c)
add_dependencies(vcard_test ical icalvcard)
target_link_libraries(vcard_test ical icalvcard)

add_executable(vcard_test_encode vcard_test_encode.c)
add_dependencies(vcard_test_encode icalvcard)
target_link_libraries(vcard_test_encode icalvcard)

add_test(NAME vcard_construct COMMAND vcard_test)
setprops(vcard_construct)

add_test(NAME vcard_encode COMMAND vcard_test_encode)
setprops(vcard_encode)
