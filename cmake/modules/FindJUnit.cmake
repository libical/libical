#
# SPDX-FileCopyrightText: 2025 Contributors to the libical project <git@github.com:libical/libical>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
# Try to find JUnit
#
#  JUnit_FOUND - system has JUnit
#  JUnit_JAR - path to the JUnit JAR file
#  JUnit::JUnit - imported target for JUnit

set_package_properties(
  JUnit
  PROPERTIES
    DESCRIPTION
      "A testing framework for Java"
    URL
      "https://junit.org"
)

include(FetchContent)

if(NOT DEFINED JUnit_FIND_VERSION)
  set(JUnit_FIND_VERSION "1.11.4")
endif()

set(_junit_jar_name "junit-platform-console-standalone-${JUnit_FIND_VERSION}.jar")
set(
  _junit_url
  "https://repo1.maven.org/maven2/org/junit/platform/junit-platform-console-standalone/${JUnit_FIND_VERSION}/${_junit_jar_name}"
)

FetchContent_Declare(junit URL "${_junit_url}" DOWNLOAD_NO_EXTRACT TRUE DOWNLOAD_NAME "${_junit_jar_name}")

FetchContent_MakeAvailable(junit)

FetchContent_GetProperties(junit SOURCE_DIR _junit_source_dir)
set(JUnit_JAR "${_junit_source_dir}/${_junit_jar_name}")

if(EXISTS "${JUnit_JAR}")
  set(JUnit_FOUND TRUE)

  if(NOT TARGET JUnit::JUnit)
    add_library(JUnit::JUnit IMPORTED INTERFACE)
    set_target_properties(
      JUnit::JUnit
      PROPERTIES
        IMPORTED_LOCATION
          "${JUnit_JAR}"
    )
  endif()

  if(NOT JUnit_FIND_QUIETLY)
    message(STATUS "Found JUnit: ${JUnit_JAR}")
  endif()
else()
  set(JUnit_FOUND FALSE)
  if(JUnit_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find JUnit")
  elseif(NOT JUnit_FIND_QUIETLY)
    message(STATUS "Could not find JUnit")
  endif()
endif()

mark_as_advanced(JUnit_JAR)

unset(_junit_jar_name)
unset(_junit_url)
unset(_junit_source_dir)
