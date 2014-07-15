# This is the top-level CMakeLists.txt file for the libical project.
#
# Pass the following variables to cmake to control the build:
# (See doc/UsingLibical.txt for more information)
#
# -DICAL_ERRORS_ARE_FATAL=[true|false]
#  Set to make icalerror_* calls abort instead of internally signaling an error
#  Default=false
#
# -DNO_WARN_DEPRECATED=[true|false]
#  Set if you DO NOT WANT to see deprecated messages.
#  Default=true
#
# -DICAL_ALLOW_EMPTY_PROPERTIES=[true|false]
#  Set to prevent empty properties from being replaced with X-LIC-ERROR properties.
#  Default=false
#
# -DICAL_UNIX_NEWLINE=[true|false]
#  Set to terminate each line with the RFC2445 specified line terminator "\\n";
#  else it will terminate lines with "\\n\\r"
#  Default=true (on non-Windows systems; else false)
#
# -DUSE_BUILTIN_TZDATA=[true|false]
#  Set to build using our own timezone data.
#  Default=false (use the system timezone data on non-Windows systems)
#  ALWAYS true on Windows systems
#
# -DSTATIC_LIBRARY=[true|false]
#  Set to build static libraries only.
#  Default=false (build shared and static libs)
#
# -DGOBJECT_INTROSPECTION=[true|false]
#  Set to build GObject introspection "typelib" files
#  Requires GObject Introspection development package v0.6.7 or higher
#  Default=false (do not generate the introspection files)
#
# -DUSE_32BIT_TIME_T=[true|false]
#  Set to build using a 32bit time_t (ignored unless building with MSVC on Windows)
#  Default=false (use the default size of time_t)
#

cmake_minimum_required(VERSION 2.8.9)

project(libical C CXX)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0005 OLD)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

enable_testing()

if(WINCE)
  find_package(Wcecompat REQUIRED)
  include_directories(${WCECOMPAT_INCLUDE_DIR})
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${WCECOMPAT_INCLUDE_DIR})
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${WCECOMPAT_LIBRARIES})
endif()

set(LIBICAL_LIB_MAJOR_VERSION "1")
set(LIBICAL_LIB_MINOR_VERSION "0")
set(LIBICAL_LIB_PATCH_VERSION "1")
set(LIBICAL_LIB_VERSION_STRING
  "${LIBICAL_LIB_MAJOR_VERSION}.${LIBICAL_LIB_MINOR_VERSION}.${LIBICAL_LIB_PATCH_VERSION}"
)

set(PROJECT_VERSION "${LIBICAL_LIB_MAJOR_VERSION}.${LIBICAL_LIB_MINOR_VERSION}")
set(PROJECT_URL "http://libical.github.io/libical/")

if(STATIC_LIBRARY)
  set(LIBICAL_STATIC 1)
  add_definitions(-DLIBICAL_STATIC)
  set(LIBRARY_TYPE STATIC)
else()
  set(LIBRARY_TYPE SHARED)
endif()

# must have Perl to create the derived stuff
find_package(Perl REQUIRED)

# libicu is highly recommended for RSCALE support
#  libicu can be found at http://www.icu-project.org
#  RSCALE info at http://tools.ietf.org/html/draft-daboo-icalendar-rscale
find_package(ICU)
if(ICU_FOUND)
  set(HAVE_LIBICU 1)
endif()
if(ICU_I18N_FOUND)
  set(HAVE_LIBICU_I18N 1)
endif()

# MSVC specific definitions
if(WIN32)
  if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DYY_NO_UNISTD_H)
    if(USE_32BIT_TIME_T)
      add_definitions(-D_USE_32BIT_TIME_T)
    endif()
  endif()
  add_definitions(-DBIG_ENDIAN=0 -DLITTLE_ENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN)
endif()

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE STRING "Include directory name" FORCE)
set(SHARE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Share directory name" FORCE)

# set the output paths
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
if(WIN32)
  set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
  set(LIB_INSTALL_DIR lib)
  set(BIN_INSTALL_DIR bin)
else()
  set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR ${LIB_DESTINATION})
endif()

if(NOT DEFINED ICAL_ERRORS_ARE_FATAL)
  set(ICAL_ERRORS_ARE_FATAL 0)
else()
  if(ICAL_ERRORS_ARE_FATAL)
    set(ICAL_ERRORS_ARE_FATAL 1)
  else()
    set(ICAL_ERRORS_ARE_FATAL 0)
  endif()
endif()

if(NOT DEFINED ICAL_UNIX_NEWLINE)
  if(WIN32 OR WINCE)
    set(ICAL_UNIX_NEWLINE 0)
  else()
    set(ICAL_UNIX_NEWLINE 1)
  endif()
else()
  if(ICAL_UNIX_NEWLINE)
    set(ICAL_UNIX_NEWLINE 1)
  else()
    set(ICAL_UNIX_NEWLINE 0)
  endif()
endif()

if(NOT DEFINED ICAL_ALLOW_EMPTY_PROPERTIES)
  set(ICAL_ALLOW_EMPTY_PROPERTIES 0)
else()
  if(ICAL_ALLOW_EMPTY_PROPERTIES)
    set(ICAL_ALLOW_EMPTY_PROPERTIES 1)
  else()
    set(ICAL_ALLOW_EMPTY_PROPERTIES 0)
  endif()
endif()

if(NOT DEFINED NO_WARN_DEPRECATED)
  set(NO_WARN_DEPRECATED 1)
else()
  if(NO_WARN_DEPRECATED)
    set(NO_WARN_DEPRECATED 1)
  else()
    set(NO_WARN_DEPRECATED 0)
  endif()
endif()

if(NOT DEFINED USE_BUILTIN_TZDATA)
  set(USE_BUILTIN_TZDATA 0)
else()
  if(USE_BUILTIN_TZDATA)
    set(USE_BUILTIN_TZDATA 1)
  else()
    set(USE_BUILTIN_TZDATA 0)
  endif()
endif()
#Always use builtin tzdata on Windows systems.
if(WIN32 OR WINCE)
  set(USE_BUILTIN_TZDATA 1)
endif()

include(ConfigureChecks.cmake)
add_definitions(-DHAVE_CONFIG_H)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

if(GOBJECT_INTROSPECTION)
  set(MIN_GOBJECT_INTROSPECTION "0.6.7")
  find_package(GObjectIntrospection ${MIN_GOBJECT_INTROSPECTION})
  if(INTROSPECTION_FOUND)
    set(HAVE_INTROSPECTION TRUE)
  else()
    message(FATAL_ERROR "You requested to build with GObject Introspection but the necessary development package is missing or too low a version (version ${MIN_GOBJECT_INTROSPECTION} or higher is required)")
  endif()
endif()

#
# Compiler settings
#
if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  include(CheckCCompilerFlag)
  check_c_compiler_flag(-Wunused-but-set-variable HAVE_GCC_UNUSED_BUT_SET)
  check_c_compiler_flag(-Wlogical-op HAVE_GCC_LOGICAL_OP)
  check_c_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GCC_POINTER_MEMACCESS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated -Wall -Wextra -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GCC_UNUSED_BUT_SET)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GCC_LOGICAL_OP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GCC_POINTER_MEMACCESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GCC_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GCC_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GCC_POINTER_MEMACCESS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wall -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GCC_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GCC_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GCC_POINTER_MEMACCESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
endif()

################# build subdirs ########################

add_subdirectory(design-data)
add_subdirectory(doc)
add_subdirectory(scripts)
add_subdirectory(test-data)
add_subdirectory(src)
add_subdirectory(examples)
if(USE_BUILTIN_TZDATA)
  # use our zoneinfo if cmake is passed -DUSE_BUILTIN_TZDATA
  add_subdirectory(zoneinfo)
endif()

########### create and install pkg-config file #########

set(VERSION "${PROJECT_VERSION}")
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")
set(PTHREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libical.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libical.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libical.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

########### Create and install the CMake Config files ##########
include(CMakePackageConfigHelpers)

configure_package_config_file(
  LibIcalConfig.cmake.in ${libical_BINARY_DIR}/LibIcalConfig.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/LibIcal
  PATH_VARS LIB_INSTALL_DIR INCLUDE_INSTALL_DIR
)

# Create a version file
write_basic_package_version_file(
  ${libical_BINARY_DIR}/LibIcalConfigVersion.cmake
  VERSION ${LIBICAL_LIB_VERSION_STRING}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES ${libical_BINARY_DIR}/LibIcalConfigVersion.cmake ${libical_BINARY_DIR}/LibIcalConfig.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/LibIcal
)

install(
  EXPORT icalTargets
  DESTINATION ${LIB_INSTALL_DIR}/cmake/LibIcal
  FILE LibIcalTargets.cmake
)
