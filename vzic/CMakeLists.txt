# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
#
# Pass the following variables to cmake to control the build:
# (See docs/HOWTO-update-zoneinfo.md for more information)
#
# -DPRODUCT_ID=[propid]
#  This variable is used as the PRODID property on the iCalendar files output.
#  It identifies the product which created the iCalendar objects.
#  So you need to substitute your own organization name and product.
#  Default=-//citadel.org//NONSGML Citadel calendar//EN
#
# -DTZID_PREFIX=[tzid_prefix]
#  This variable is used to create unique IDs for each VTIMEZONE component.
#  The prefix is put before each timezone city name. It should start and end
#  with a '/'. The first part, i.e. 'myorganization.org' below, should be
#  a unique vendor ID, e.g. use a hostname. The part after that can be
#  anything you want. We use a date and version number for libical. The %D
#  gets expanded to today's date. There is also a vzic-merge.pl which can be
#  used to merge changes into a master set of VTIMEZONEs. If a VTIMEZONE has
#  changed, it bumps the version number on the end of this prefix. */
#  Default=/citadel.org/%D_1/
#
# -DCREATE_SYMLINK=[true|false]
#  This option is used to indicate how timezone aliases (indicated by a Link line
#  in Olson files) should be generated: The default is to symbolically link
#  the Link zone file to its authoritative zone. Alternatively, if set to False,
#  a VTIMEZONE file is generated for each Link.
#  Default=False
#
# -DIGNORE_TOP_LEVEL_LINK=[true|false]
#  This option indicates if top-level timezone aliases (a timezone name without
#  any '/' such as "EST5EDT") should be ignored. If False, a VTIMEZONE is
#  generated also for top-level aliases. This option only has
#  an effect if CREATE_SYMLINK is False, and mainly is useful for backward
#  compatibility with previous vzic versions.
#  Default=False
#

if(NOT PRODUCT_ID)
  set(PRODUCT_ID "-//citadel.org//NONSGML Citadel calendar//EN")
endif()
message(STATUS "PRODUCT_ID is ${PRODUCT_ID}")
add_definitions(-DPRODUCT_ID=\"${PRODUCT_ID}\")

if(NOT TZID_PREFIX)
  set(TZID_PREFIX "/citadel.org/%D_1/")
endif()
message(STATUS "TZID_PREFIX is ${TZID_PREFIX}")
add_definitions(-DTZID_PREFIX=\"${TZID_PREFIX}\")

option(CREATE_SYMLINK "Symbolically link the Link zone file to its authoritative zone" False)
if(CREATE_SYMLINK)
  add_definitions(-DCREATE_SYMLINK=1)
else()
  add_definitions(-DCREATE_SYMLINK=0)
endif()

option(
  IGNORE_TOP_LEVEL_LINK
  "Ignore top-level timezone aliases (a timezone name without any '/' such as EST5EDT)"
  False
)
if(IGNORE_TOP_LEVEL_LINK)
  add_definitions(-DIGNORE_TOP_LEVEL_LINK=1)
else()
  add_definitions(-DIGNORE_TOP_LEVEL_LINK=0)
endif()

set(
  VZIC_SRCS
  vzic.c
  vzic.h
  vzic-dump.c
  vzic-dump.h
  vzic-output.c
  vzic-output.h
  vzic-parse.c
  vzic-parse.h
)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libical
)

add_executable(vzic ${VZIC_SRCS})
target_compile_options(vzic PRIVATE ${GLIB_CFLAGS})
target_link_libraries(vzic PRIVATE ${GLIB_LIBRARIES})

# test-vzic is very slow and should only be run by-hand
add_executable(
  test-vzic
  test-vzic.c
  ${VZIC_SRCS}
)
target_compile_definitions(test-vzic PRIVATE -DVZIC_LIBRARY)
target_compile_options(test-vzic PRIVATE ${GLIB_CFLAGS})
target_link_libraries(
  test-vzic
  PRIVATE
    ${GLIB_LIBRARIES}
    ical
)

if(LIBICAL_BUILD_TESTING AND NOT WIN32)
  # not Windows, because we currently rely on the 'diff' command and redirecting output to /dev/null
  set(tzYear "2025c")
  set(
    diffArgs
    -ILAST-MODIFIED:
    -ITZID:
    -ITZID-ALIAS-OF:
  )
  add_custom_command(
    TARGET vzic
    POST_BUILD
    COMMAND
      vzic --olson-dir ${PROJECT_SOURCE_DIR}/test-data/tzdata${tzYear} --output-dir
      ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen >/dev/null
    BYPRODUCTS
      ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen
    #VERBATIM #comment this so we can redirect any vzic output to /dev/null
  )
  add_test(
    NAME zoneinfo-cmp
    COMMAND
      diff --strip-trailing-cr ${diffArgs} ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen
      ${PROJECT_SOURCE_DIR}/test-data/zoneinfo${tzYear}
  )

  # test the "pure" (ie. with historical data) version
  add_custom_command(
    TARGET vzic
    POST_BUILD
    COMMAND
      vzic --pure --olson-dir ${PROJECT_SOURCE_DIR}/test-data/tzdata${tzYear} --output-dir
      ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen-pure >/dev/null
    BYPRODUCTS
      ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen-pure
    #VERBATIM #comment this so we can redirect any vzic output to /dev/null
  )
  add_test(
    NAME zoneinfo-cmp-pure
    COMMAND
      diff --strip-trailing-cr ${diffArgs} ${CMAKE_CURRENT_BINARY_DIR}/zoneinfo-gen-pure
      ${PROJECT_SOURCE_DIR}/test-data/zoneinfo${tzYear}-pure
  )
endif()
