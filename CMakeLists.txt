# This is the top-level CMakeLists.txt file for the libical project.
#
# Pass the following variables to cmake to control the build:
# (See doc/UsingLibical.md for more information)
#
# -DWITH_CXX_BINDINGS=[true|false]
#  Build the C++ bindings.
#  Default=true
#
# -DICAL_ERRORS_ARE_FATAL=[true|false]
#  Set to make icalerror_* calls abort instead of internally signaling an error
#  Default=false
#  Notes:
#   Change the behavior at runtime using the icalerror_set_errors_are_fatal() function.
#   Query the behavior at runtime using the icalerror_get_errors_are_fatal() function.
#
# -DICAL_ALLOW_EMPTY_PROPERTIES=[true|false]
#  Set to prevent empty properties from being replaced with X-LIC-ERROR properties.
#  Default=false
#
# -DUSE_BUILTIN_TZDATA=[true|false]
#  Set to build using our own timezone data.
#  Default=false (use the system timezone data on non-Windows systems)
#  ALWAYS true on Windows systems
#
# -DSTATIC_ONLY=[true|false]
#  Set to build static libraries only.
#  Not available for GObject Introspection and Vala "vapi"
#  Default=false (build shared and static libs)
#
# -DSHARED_ONLY=[true|false]
#  Set to build shared (dynamic) libraries only.
#  Default=false (build shared and static libs)
#  Takes precedence over STATIC_ONLY
#
# -DGOBJECT_INTROSPECTION=[true|false]
#  Set to build GObject introspection "typelib" files
#  Requires GObject Introspection development package (version MIN_GOBJECT_INTROSPECTION)
#  Default=false (do not generate the introspection files)
#
# -DICAL_BUILD_DOCS=[true|false]
#  Configure for the API documentation and User Manual.  The 'docs' target will not be available.
#  Default=true
#
# -DICAL_GLIB_VAPI=[true|false]
#  Set to build Vala "vapi" files
#  Requires Vala package
#  Default=false (build the libical-glib interface)
#
# -DICAL_GLIB=[true|false]
#  Set to build libical-glib (GObject) interface
#  Requires glib 2.0 development package (version MIN_GLIB).
#  Requires libxml2.0 development package (version MIN_LIBXML).
#  Default=true (build the libical-glib interface)
#
# -DENABLE_GTK_DOC=[true|false]
#  Set to build libical-glib developer documentation
#  Requires gtk-doc and ICAL_BUILD_DOCS option to be true.
#  Default=true (build gtk-doc developer documentation)
#
# -DUSE_32BIT_TIME_T=[true|false]
#  Set to build using a 32bit time_t (ignored unless building with MSVC on Windows)
#  Default=false (use the default size of time_t)
#
# -DLIBICAL_BUILD_TESTING=[true|false]
#  Set to build the test suite
#  Default=true
#
# -DLIBICAL_BUILD_EXAMPLES=[true|false]
#  Set to build the examples
#  Default=true
#

## Special CMake Options for Developers
#
# -DABI_DUMPER=[true|false]
#  Build for the abi-dumper (requires gcc)
#  Default=false
#
# -DADDRESS_SANITIZER=[true|false]
#  Build with the address sanitizer (requires gcc or clang)
#
# -DTHREAD_SANITIZER=[true|false]
#  Build with the thread sanitizer (requires gcc or clang)
#

cmake_minimum_required(VERSION 3.5.0) #first line, to shutup a cygwin warning
project(libical C) #CXX is optional for the bindings

cmake_policy(SET CMP0003 NEW)
if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) #don't warn about ICU_ROOT envvar
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Kitware/modules")

#Include CMake capabilities
include(FeatureSummary)

# Enable the test harness
enable_testing()

if(WINCE)
  find_package(Wcecompat REQUIRED)
  include_directories(${WCECOMPAT_INCLUDE_DIR})
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${WCECOMPAT_INCLUDE_DIR})
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${WCECOMPAT_LIBRARIES})
endif()

set(LIBICAL_LIB_MAJOR_VERSION "3")
set(LIBICAL_LIB_MINOR_VERSION "0")
set(LIBICAL_LIB_PATCH_VERSION "19")
set(LIBICAL_LIB_VERSION_STRING
  "${LIBICAL_LIB_MAJOR_VERSION}.${LIBICAL_LIB_MINOR_VERSION}.${LIBICAL_LIB_PATCH_VERSION}"
)

set(PROJECT_VERSION "${LIBICAL_LIB_MAJOR_VERSION}.${LIBICAL_LIB_MINOR_VERSION}")
set(PROJECT_URL "https://libical.github.io/libical/")

# library build types
set(LIBRARY_TYPE SHARED)

########################################################

option(WITH_CXX_BINDINGS "Build the C++ bindings." True)
if(WITH_CXX_BINDINGS)
  enable_language(CXX)
  if(CMAKE_CXX_COMPILER)
    add_definitions(-DWITH_CXX_BINDINGS)
  else()
    message(STATUS
      "Warning: Building the C++ bindings is not possible since a C++ compiler could not be found. "
      "Turning-off C++ bindings."
    )
    set(WITH_CXX_BINDINGS False)
  endif()
endif()
add_feature_info(
  "Option WITH_CXX_BINDINGS"
  WITH_CXX_BINDINGS
  "build the C++ bindings. Requires a C++ compiler"
)

option(STATIC_ONLY "Build static libraries only.")
add_feature_info(
  "Option STATIC_ONLY"
  STATIC_ONLY
  "build static libraries only"
)
if(STATIC_ONLY)
  set(LIBRARY_TYPE STATIC)
endif()

option(SHARED_ONLY "Build shared (dynamic) libraries only. Takes precedence over STATIC_ONLY")
add_feature_info(
  "Option SHARED_ONLY"
  SHARED_ONLY
  "build shared libraries only"
)
if(SHARED_ONLY)
  set(STATIC_ONLY False)
  set(LIBRARY_TYPE SHARED)
endif()

if(NOT STATIC_ONLY AND NOT SHARED_ONLY)
  add_feature_info(
    "Build types"
    TRUE
    "build both shared and static libraries"
  )
endif()

if(STATIC_ONLY)
  add_definitions(
    -DLIBICAL_ICAL_STATIC_DEFINE
    -DLIBICAL_ICALSS_STATIC_DEFINE
    -DLIBICAL_VCAL_STATIC_DEFINE
  )
endif()

# must have Perl to create the derived stuff
find_package(Perl REQUIRED)
set_package_properties(Perl PROPERTIES
  TYPE REQUIRED
  PURPOSE "Required by the libical build system."
)

# Ensure finding 64bit libs when using 64-bit compilers
if(CMAKE_CL_64)
  set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS True)
endif()

# libicu is highly recommended for RSCALE support
#  libicu can be found at http://www.icu-project.org
#  RSCALE info at http://tools.ietf.org/html/rfc7529
if(DEFINED ENV{ICU_BASE}) # keep backwards compatibility for ICU_BASE
  set(ICU_ROOT $ENV{ICU_BASE})
endif()
if(NOT DEFINED ENV{ICU_ROOT} AND APPLE)
  #Use the homebrew version. MacOS provided ICU doesn't provide development files
  set(ICU_ROOT "/usr/local/opt/icu4c")
endif()
find_package(ICU COMPONENTS uc i18n data)
set_package_properties(ICU PROPERTIES
  TYPE RECOMMENDED
  PURPOSE "For RSCALE (RFC7529) support"
)
add_feature_info(
  "RSCALE support (RFC7529)"
  ICU_FOUND
  "build in RSCALE support"
)
if(ICU_FOUND)
  set(REQUIRES_PRIVATE_ICU "Requires.private: icu-i18n") #for libical.pc
  set(HAVE_LIBICU 1)
  if(ICU_VERSION VERSION_GREATER 50)
    set(HAVE_ICU_DANGI TRUE)
  else()
    set(HAVE_ICU_DANGI FALSE)
  endif()
  if(ICU_GENCCODE_EXECUTABLE)
    get_filename_component(ICU_EXEC ${ICU_GENCCODE_EXECUTABLE} DIRECTORY)
  elseif(ICU_UCONV_EXECUTABLE)
    get_filename_component(ICU_EXEC ${ICU_UCONV_EXECUTABLE} DIRECTORY)
  elseif(ICU_ICUINFO_EXECUTABLE)
    get_filename_component(ICU_EXEC ${ICU_ICUINFO_EXECUTABLE} DIRECTORY)
  elseif(ICU_ICU-CONFIG_EXECUTABLE)
    get_filename_component(ICU_EXEC ${ICU_ICU-CONFIG_EXECUTABLE} DIRECTORY)
  elseif(ICU_MAKECONV_EXECUTABLE)
    get_filename_component(ICU_EXEC ${ICU_MAKECONV_EXECUTABLE} DIRECTORY)
  else()
    message(FATAL_ERROR
      "Unable locate the ICU runtime path. Is your ICU installation broken?")
  endif()
  set(ICU_BINARY_DIR ${ICU_EXEC} CACHE STRING DOC "Runtime binaries directory for the ICU library")
endif()

# compile in Berkeley DB support
if(NOT "$ENV{BerkeleyDB_ROOT_DIR}")
  if(APPLE)
    #Use the homebrew version. Xcode's version doesn't work for us.
    set(BerkeleyDB_ROOT_DIR "/usr/local/opt/berkeley-db")
  endif()
endif()
find_package(BerkeleyDB)
set_package_properties(BerkeleyDB PROPERTIES
  TYPE OPTIONAL
  PURPOSE "For Berkeley DB storage support"
)
add_feature_info(
  "Berkeley DB storage support"
  BerkeleyDB_FOUND
  "build in support for Berkeley DB storage"
)
if(BerkeleyDB_FOUND)
  set(HAVE_BDB True)
  add_definitions(-DDB_DBM_HSEARCH=0) #set to 1 if hsearch support is needed
  #for compatibility to our old FindBDB
  set(BDB_FOUND True)
  set(BDB_INCLUDE_DIR ${BerkeleyDB_INCLUDE_DIRS})
  set(BDB_LIBRARY ${BerkeleyDB_LIBRARIES})
endif()

# MSVC specific definitions
if(WIN32)
  if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DYY_NO_UNISTD_H)
    option(USE_32BIT_TIME_T "Build using a 32bit time_t (ignored unless building with MSVC on Windows).")
    add_feature_info(
      "Option USE_32BIT_TIME_T"
      USE_32BIT_TIME_T
      "build using 32-bit time_t"
    )
    if(USE_32BIT_TIME_T)
      add_definitions(-D_USE_32BIT_TIME_T)
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290") #C++ exception specification ignored
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4068") #unknown pragma
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4028") #formal parameter differs
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4068") #unknown pragma
  endif()
  add_definitions(-DBIG_ENDIAN=0 -DLITTLE_ENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN)
endif()

# Use GNUInstallDirs

include(GNUInstallDirs)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING "Library directory name" FORCE)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING "Include directory name" FORCE)
set(SHARE_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR} CACHE STRING "Share directory name")

# set the output paths
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
if(WIN32)
  set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
  set(LIB_INSTALL_DIR lib)
  set(BIN_INSTALL_DIR bin)
else()
  set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR ${LIB_DESTINATION})
endif()

option(ICAL_ERRORS_ARE_FATAL "icalerror_* calls will abort instead of internally signaling an error.")
add_feature_info(
  "Option ICAL_ERRORS_ARE_FATAL"
  ICAL_ERRORS_ARE_FATAL
  "icalerror_* calls will abort instead of internally signaling an error"
)
if(ICAL_ERRORS_ARE_FATAL)
  set(ICAL_ERRORS_ARE_FATAL 1)
else()
  set(ICAL_ERRORS_ARE_FATAL 0)
endif()

option(ICAL_ALLOW_EMPTY_PROPERTIES "Prevent empty properties from being replaced with X-LIC-ERROR properties.")
add_feature_info(
  "Option ICAL_ALLOW_EMPTY_PROPERTIES"
  ICAL_ALLOW_EMPTY_PROPERTIES
  "prevents empty properties from being replaced with X-LIC-ERROR properties"
)
if(ICAL_ALLOW_EMPTY_PROPERTIES)
  set(ICAL_ALLOW_EMPTY_PROPERTIES 1)
else()
  set(ICAL_ALLOW_EMPTY_PROPERTIES 0)
endif()

option(USE_BUILTIN_TZDATA "(Careful) Build using libical's built-in timezone data, else use the system timezone data on non-Windows systems. ALWAYS true on Windows. Non-Windows users should know what they're doing if they choose not to use system provided timezone data. The libical project does not guarantee that the built-in timezone data is up-to-date.")
mark_as_advanced(USE_BUILTIN_TZDATA)
if(USE_BUILTIN_TZDATA)
  set(USE_BUILTIN_TZDATA 1)
else()
  set(USE_BUILTIN_TZDATA 0)
endif()
add_feature_info(
  "Option USE_BUILTIN_TZDATA"
  USE_BUILTIN_TZDATA
  "use our own timezone data rather then the system timezone data"
)
if(WIN32 OR WINCE)
  #Always use builtin tzdata on Windows systems.
  if(NOT USE_BUILTIN_TZDATA)
    message(STATUS
      "Currently unable to use system tzdata on Windows. Falling back to builtin tzdata."
    )
    set(USE_BUILTIN_TZDATA 1)
  endif()
endif()

include(ConfigureChecks.cmake)
add_definitions(-DHAVE_CONFIG_H)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if(NDEBUG)
  add_definitions(-DNDEBUG)
endif()

set(INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

#Look for PkgConfig
#if not found then cannot proceed for GLib+LibXML or GObjectIntrospection.
#(unless/until they can be handled without pkg-config some day)
find_package(PkgConfig QUIET)

set(MIN_GOBJECT_INTROSPECTION "0.6.7")
option(GOBJECT_INTROSPECTION "Build GObject introspection \"typelib\" files. Requires GObject Introspection development package ${MIN_GOBJECT_INTROSPECTION} or higher.")
add_feature_info(
  "Option GOBJECT_INTROSPECTION"
  GOBJECT_INTROSPECTION
  "build GObject introspection \"typelib\" files"
)
if(GOBJECT_INTROSPECTION)
  if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR
      "You are attempting to build with GObject Introspection enabled, however that option is "
      "not supported unless pkg-config can be found.  Please install pkg-config and try again. "
      "You can disable GObject Introspection by passing -DGOBJECT_INTROSPECTION=False to cmake.")
  endif()

  find_package(GObjectIntrospection ${MIN_GOBJECT_INTROSPECTION})
  set_package_properties(GObjectIntrospection PROPERTIES
    TYPE OPTIONAL
    URL "https://wiki.gnome.org/Projects/GObjectIntrospection"
    PURPOSE "Needed in order to build the GObject introspection \"typelib\" files."
  )
  if(GObjectIntrospection_FOUND)
    set(HAVE_INTROSPECTION TRUE)
    if(STATIC_ONLY)
      message(FATAL_ERROR
        "You are attempting to build with GObject Introspection enabled, "
        "however that option is not supported when building static libraries only. "
        "Please disable the static only option to cmake (-DSTATIC_ONLY=False) "
        "if you really want to build with GObject Introspection. Alternatively, "
        "you can disable GObject Introspection (by passing -DGOBJECT_INTROSPECTION=False to cmake).")
    endif()
  else()
    message(FATAL_ERROR
      "You requested to build with GObject introspection, but the necessary development package "
      "is missing or too low a version (GObjectIntrospection ${MIN_GOBJECT_INTROSPECTION} or higher is required). "
      "You can disable GObject Introspection by passing -DGOBJECT_INTROSPECTION=False to cmake.")
  endif()
endif()

option(ICAL_BUILD_DOCS "Build documentation" True)
add_feature_info(
  "Option ICAL_BUILD_DOCS"
  ICAL_BUILD_DOCS
  "build API documentation and reference manual"
)

option(ICAL_GLIB_VAPI "Build Vala \"vapi\" files.")
add_feature_info(
  "Option ICAL_GLIB_VAPI"
  ICAL_GLIB_VAPI
  "build Vala \"vapi\" files"
)
if(ICAL_GLIB_VAPI)
  if(STATIC_ONLY)
    message(FATAL_ERROR
      "You are attempting to build the Vala api, however that option is not supported "
      "when building static libraries only. "
      "Please disable the static only option (-DSTATIC_ONLY=False) "
      "if you really want to build the Vala api. Alternatively, "
      "you can disable this feature (by passing -DICAL_GLIB_VAPI=False to cmake).")
  endif()
  if(NOT GOBJECT_INTROSPECTION)
    message(FATAL_ERROR
      "You requested to build the Vala vapi but have not enabled the GObject Introspection. "
      "Please try again also passing -DGOBJECT_INTROSPECTION=True to cmake.")
  endif()

  find_program(VALAC valac DOC "the Vala compiler")
  if(NOT VALAC)
    message(FATAL_ERROR
      "valac, the Vala compiler was not found. "
      "Install it or disable Vala bindings with -DICAL_GLIB_VAPI=False.")
  endif()

  find_program(VAPIGEN vapigen DOC "tool to generate the Vala API")
  if(NOT VAPIGEN)
    message(FATAL_ERROR
      "vapigen, the tool for generating the Vala API was not found. "
      "Install it or disable Vala bindings with -DICAL_GLIB_VAPI=False.")
  endif()
endif()

set(MIN_GLIB "2.32")
set(MIN_LIBXML "2.7.3")
option(ICAL_GLIB "Build libical-glib interface. Requires glib ${MIN_GLIB} and libxml ${MIN_LIBXML} development packages or higher." True)
add_feature_info(
  "Option ICAL_GLIB"
  ICAL_GLIB
  "build libical-glib interface"
)
if(ICAL_GLIB)
  if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR
      "You requested to build libical-glib, however that option is not supported "
      "unless pkg-config can be found. Please install pkg-config and try again. "
      "Alternatively, disable the libical-glib build (by passing -DICAL_GLIB=False to cmake).")
  endif()

  find_package(GLib ${MIN_GLIB})
  set_package_properties(GLib PROPERTIES
    TYPE OPTIONAL
    PURPOSE "For the optional libical-glib interface"
  )
  find_package(LibXML ${MIN_LIBXML})
  set_package_properties(LibXML PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "a library providing XML and HTML support"
    URL "http://xmlsoft.org"
    PURPOSE "For the optional libical-glib interface"
  )
  if(GLIB_FOUND AND LIBXML_FOUND)
    set(HAVE_GLIB TRUE)
  elseif(GLIB_FOUND)
    message(FATAL_ERROR
      "You requested to build libical-glib, but the necessary development package "
      "is missing or too low a version (libxml ${MIN_LIBXML} or higher is required). "
      "Alternatively, disable the libical-glib build (by passing -DICAL_GLIB=False to cmake).")
  elseif(LIBXML_FOUND)
    message(FATAL_ERROR
      "You requested to build libical-glib, but the necessary development package "
      "is missing or too low a version (glib ${MIN_GLIB} or higher is required. "
      "Alternatively, disable the libical-glib build (by passing -DICAL_GLIB=False to cmake).")
  else()
    message(FATAL_ERROR
      "You requested to build libical-glib, but the necessary development packages "
      "are missing or too low a version "
      "(glib ${MIN_GLIB} and libxml ${MIN_LIBXML} or higher are required). "
      "Alternatively, disable the libical-glib build (by passing -DICAL_GLIB=False to cmake).")
  endif()
else()
  #to quiet a warning about unused manually-specified variables
  set(ENABLE_GTK_DOC False)
endif()

#
# Compiler settings
#
if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  include(CheckCCompilerFlag)
  check_c_compiler_flag(-Wunused-but-set-variable HAVE_GCC_UNUSED_BUT_SET)
  check_c_compiler_flag(-Wlogical-op HAVE_GCC_LOGICAL_OP)
  check_c_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GCC_POINTER_MEMACCESS)
  check_c_compiler_flag(-Wformat-security HAVE_GCC_FORMAT_SECURITY)
  check_c_compiler_flag(-Wredundant-decls HAVE_GCC_REDUNDANT_DECLS)
  check_c_compiler_flag(-Wunreachable-code HAVE_GCC_UNREACHABLE_CODE)
  check_c_compiler_flag(-Wvarargs HAVE_GCC_VARARGS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wno-deprecated -Wall -Wno-unknown-pragmas -Wextra -Winit-self -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wtype-limits -Wwrite-strings -Werror=return-type")
  #for more modern C
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wold-style-definition -Wstrict-prototypes")
  if(HAVE_GCC_UNUSED_BUT_SET)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GCC_LOGICAL_OP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GCC_POINTER_MEMACCESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
  if(HAVE_GCC_FORMAT_SECURITY)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat -Wformat-security")
  endif()
  if(HAVE_GCC_REDUNDANT_DECLS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
  endif()
  if(HAVE_GCC_UNREACHABLE_CODE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")
  endif()
  if(HAVE_GCC_VARARGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvarargs")
  endif()
  if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE -D_GNU_SOURCE")
  endif()
endif()
if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
endif()
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  check_cxx_compiler_flag(-Wredundant-decls HAVE_GXX_REDUNDANT_DECLS)
  check_cxx_compiler_flag(-Wunreachable-code HAVE_GXX_UNREACHABLE_CODE)
  check_cxx_compiler_flag(-Wvarargs HAVE_GXX_VARARGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Weffc++ -Wno-deprecated -Wall -Wextra -Woverloaded-virtual -Winit-self -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wtype-limits -Wwrite-strings -Werror=return-type")
  if(HAVE_GXX_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GXX_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GXX_POINTER_MEMACCESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
  if(HAVE_GXX_REORDER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreorder")
  endif()
  if(HAVE_GXX_FORMAT_SECURITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security")
  endif()
  if(HAVE_GXX_REDUNDANT_DECLS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
  endif()
  if(HAVE_GXX_UNREACHABLE_CODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
  endif()
  if(HAVE_GXX_VARARGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvarargs")
  endif()
  if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE -D_GNU_SOURCE")
  endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################ Developer Options #####################
option(ABI_DUMPER "(Developer-only) Build for abi-dumper." False)
mark_as_advanced(ABI_DUMPER)
if(ABI_DUMPER)
  if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "-g -Og")
    set(CMAKE_CXX_FLAGS "-g -Og")
  else()
    message(FATAL_ERROR
      "You are trying to build for the abi-dumper using a non-GCC compiler.")
  endif()
endif()

option(ADDRESS_SANITIZER "(Developer-only) Build with the address sanitizer." False)
mark_as_advanced(ADDRESS_SANITIZER)
if(ADDRESS_SANITIZER)
  if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g -DADDRESS_SANITIZER")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -DADDRESS_SANITIZER")
  else()
    message(FATAL_ERROR
      "You are trying to build with the address sanitizer using a non-GCC or Clang compiler.")
  endif()
  if(GOBJECT_INTROSPECTION)
    message(STATUS
      "Warning: Building the address sanitizer with GObject Introspection is is not supported. "
      "Turning-off GObject Introspection."
    )
    set(HAVE_INTROSPECTION False)
  endif()
  if(ICAL_GLIB)
    message(STATUS
      "Warning: Building the address sanitizer with the GObject interface is not supported. "
      "Turning-off the GObject interface."
    )
    set(ICAL_GLIB False)
  endif()
endif()

option(THREAD_SANITIZER "(Developer-only) Build with the thread sanitizer." False)
mark_as_advanced(THREAD_SANITIZER)
if(THREAD_SANITIZER)
  if(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -O1 -g -DTHREAD_SANITIZER")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -O1 -g -DTHREAD_SANITIZER")
  else()
    message(FATAL_ERROR
      "You are trying to build with the thread sanitizer using a non-GCC or Clang compiler.")
  endif()
  if(GOBJECT_INTROSPECTION)
    message(STATUS
      "Warning: Building the thread sanitizer with GObject Introspection is is not supported. "
      "Turning-off GObject Introspection."
    )
    set(HAVE_INTROSPECTION False)
  endif()
  if(ICAL_GLIB)
    message(STATUS
      "Warning: Building the thread sanitizer with the GObject interface is not supported. "
      "Turning-off the GObject interface."
    )
    set(ICAL_GLIB False)
  endif()
endif()

#some test programs need to know if we are using 32-bit time
if(SIZEOF_TIME_T EQUAL 4)
  set(USE_32BIT_TIME_T TRUE)
endif()

option(LIBICAL_BUILD_TESTING "Build tests." True)
add_feature_info(
  "Option LIBICAL_BUILD_TESTING"
  LIBICAL_BUILD_TESTING
  "build tests"
)

################# build subdirs ########################

add_subdirectory(design-data)
add_subdirectory(scripts)
add_subdirectory(test-data)
add_subdirectory(src)
if(LIBICAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
if(USE_BUILTIN_TZDATA)
  # use our zoneinfo if cmake is passed -DUSE_BUILTIN_TZDATA
  add_subdirectory(zoneinfo)
endif()

if(ICAL_BUILD_DOCS)
  add_subdirectory(doc) # needs to go last, for the build source files
endif()

########### create and install pkg-config file #########

set(VERSION "${LIBICAL_LIB_VERSION_STRING}")
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
if(IS_ABSOLUTE ${LIB_INSTALL_DIR})
  set(libdir "${LIB_INSTALL_DIR}")
else()
  set(libdir "\${exec_prefix}/${LIB_INSTALL_DIR}")
endif()
if(IS_ABSOLUTE ${INCLUDE_INSTALL_DIR})
  set(includedir "${INCLUDE_INSTALL_DIR}")
else()
  set(includedir "\${prefix}/include")
endif()
set(PTHREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/libical.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libical.pc
  @ONLY
)
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

########## By popular demand, add an uninstall target ##########

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE
    @ONLY
  )

  add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
  )
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
