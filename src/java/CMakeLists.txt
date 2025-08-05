# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

########### next target ###############

include(UseJava)

set(
  ical_jni_LIB_SRCS
  jlibical_consts_cxx.h
  jlibical_utils_cxx.cpp
  jlibical_utils_cxx.h
  jniICalDurationType_cxx.cpp
  jniICalDurationType_cxx.h
  jniICalPeriodType_cxx.cpp
  jniICalPeriodType_cxx.h
  jniICalRecurrenceType_cxx.cpp
  jniICalRecurrenceType_cxx.h
  jniICalTimeType_cxx.cpp
  jniICalTimeType_cxx.h
  jniICalTriggerType_cxx.cpp
  jniICalTriggerType_cxx.h
  net_cp_jlibical_ICalParameter_cxx.cpp
  net_cp_jlibical_ICalParameter_cxx.h
  net_cp_jlibical_ICalProperty_cxx.cpp
  net_cp_jlibical_ICalProperty_cxx.h
  net_cp_jlibical_ICalValue_cxx.cpp
  net_cp_jlibical_ICalValue_cxx.h
  net_cp_jlibical_VComponent_cxx.cpp
  net_cp_jlibical_VComponent_cxx.h
)

add_library(
  ical_jni
  ${LIBRARY_TYPE}
  ${ical_jni_LIB_SRCS}
)

target_include_directories(
  ical_jni
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libical
    ${CMAKE_BINARY_DIR}/src/libical
    ${JNI_INCLUDE_DIRS}
)

target_link_libraries(ical_jni ical_cxx)

if(NOT ANDROID)
  set_target_properties(
    ical_jni
    PROPERTIES
      VERSION
        ${CMAKE_PROJECT_VERSION}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()

file(GLOB JAVA_SOURCES ${CMAKE_SOURCE_DIR}/src/java/*.java)

file(
  WRITE
  ${CMAKE_CURRENT_BINARY_DIR}/manifest.txt
  "Manifest-Version: 1.0\n"
  "Created-By: CMake\n"
  "Implementation-Title: libical Java Bindings\n"
)

add_jar(
  libical_jar
  SOURCES
    ${JAVA_SOURCES}
  OUTPUT_NAME libical
  MANIFEST
  ${CMAKE_CURRENT_BINARY_DIR}/manifest.txt
  GENERATE_NATIVE_HEADERS ical_jni_headers
)

install(TARGETS ical_jni EXPORT icalTargets DESTINATION lib)

########### install files ###############

install(FILES DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${CMAKE_BINARY_DIR}/src/java/libical.jar DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
