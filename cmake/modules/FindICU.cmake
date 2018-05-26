# Finds the International Components for Unicode (ICU) Library
#
#  ICU_FOUND          - True if ICU found.
#  ICU_I18N_FOUND     - True if ICU's internationalization library found.
#  ICU_BINARY_DIR     - Directory with the runtime binaries
#  ICU_INCLUDE_DIR    - Directory to include to get ICU headers
#                       Note: always include ICU headers as, e.g.,
#                       unicode/utypes.h
#  ICU_LIBRARY        - Library to link against for the common ICU
#  ICU_I18N_LIBRARY   - Library to link against for ICU internationaliation
#                       (note: in addition to ICU_LIBRARY)
#  ICU_VERSION        - ICU version MAJOR.MINOR
#  ICU_MAJOR_VERSION  - ICU major version
#  ICO_MINOR_VERSION  - ICU minor version
#

set_package_properties(ICU PROPERTIES
  DESCRIPTION "libicu (International Components for Unicode) libraries"
  URL "http://www.icu-project.org"
)

if(WIN32)
  file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _program_FILES_DIR)
endif()

file(TO_NATIVE_PATH "$ENV{ICU_BASE}" icu_root)
if(DEFINED ICU_BASE)
  file(TO_NATIVE_PATH "${ICU_BASE}" icu_root)
else()
  find_package(PkgConfig QUIET)
  pkg_check_modules(PC_LibICU QUIET icu-i18n)
endif()

# Look for the header file.
find_path(
  ICU_INCLUDE_DIR
  NAMES unicode/utypes.h
  HINTS
  ${icu_root}/include
  ${_program_FILES_DIR}/icu/include
  ${PC_LibICU_INCLUDEDIR}
  /usr/local/opt/icu4c/include
  DOC "Include directory for the ICU library"
)
mark_as_advanced(ICU_INCLUDE_DIR)

# Look for the library.
find_library(
  ICU_LIBRARY
  NAMES icuuc cygicuuc cygicuuc32
  HINTS
  ${icu_root}/lib64/
  ${icu_root}/lib/
  ${_program_FILES_DIR}/icu/lib64/
  ${_program_FILES_DIR}/icu/lib/
  ${PC_LibICU_LIBDIR}
  /usr/local/opt/icu4c/lib/
  DOC "Libraries to link against for the common parts of ICU"
)
mark_as_advanced(ICU_LIBRARY)

# Look for the binary path.
find_program(
  tmp_DIR
  NAMES genccode uconv
  PATHS
  ${icu_root}/bin64/
  ${icu_root}/bin/
  ${_program_FILES_DIR}/icu/bin64/
  ${_program_FILES_DIR}/icu/bin/
  /usr/local/opt/icu4c/bin/
)
get_filename_component(tmp_DIR ${tmp_DIR} DIRECTORY)
set(ICU_BINARY_DIR ${tmp_DIR} CACHE DOC "Runtime binaries directory for the ICU library")
mark_as_advanced(ICU_BINARY_DIR)

# Copy the results to the output variables.
if(ICU_INCLUDE_DIR AND ICU_LIBRARY)
  set(ICU_FOUND 1)
  set(ICU_LIBRARIES ${ICU_LIBRARY})
  set(ICU_INCLUDE_DIRS ${ICU_INCLUDE_DIR})

  set(ICU_VERSION 0)
  set(ICU_MAJOR_VERSION 0)
  set(ICU_MINOR_VERSION 0)
  file(READ "${ICU_INCLUDE_DIR}/unicode/uvernum.h" _ICU_VERSION_CONENTS)
  string(REGEX REPLACE ".*#define U_ICU_VERSION_MAJOR_NUM ([0-9]+).*" "\\1" ICU_MAJOR_VERSION "${_ICU_VERSION_CONENTS}")
  string(REGEX REPLACE ".*#define U_ICU_VERSION_MINOR_NUM ([0-9]+).*" "\\1" ICU_MINOR_VERSION "${_ICU_VERSION_CONENTS}")

  set(ICU_VERSION "${ICU_MAJOR_VERSION}.${ICU_MINOR_VERSION}")

  # Look for the ICU internationalization libraries
  find_library(
    ICU_I18N_LIBRARY
    NAMES icuin icui18n cygicuin cygicuin32
    HINTS
    ${icu_root}/lib/
    ${_program_FILES_DIR}/icu/lib/
    ${PC_LibICU_LIBDIR}
    /usr/local/opt/icu4c/lib/
    DOC "Libraries to link against for ICU internationalization"
  )
  mark_as_advanced(ICU_I18N_LIBRARY)
  if(ICU_I18N_LIBRARY)
    set(ICU_I18N_FOUND 1)
    set(ICU_LIBRARIES "${ICU_LIBRARIES} ${ICU_I18N_LIBRARY}")
  else()
    set(ICU_I18N_FOUND 0)
  endif()
else()
  set(ICU_FOUND 0)
  set(ICU_I18N_FOUND 0)
  set(ICU_INCLUDE_DIRS)
  set(ICU_LIBRARIES)
  set(ICU_VERSION)
  set(ICU_MAJOR_VERSION)
  set(ICU_MINOR_VERSION)
endif()

if(ICU_FOUND)
  if(NOT ICU_FIND_QUIETLY)
    message(STATUS "Found ICU version ${ICU_VERSION}")
    message(STATUS "Found ICU header files in ${ICU_INCLUDE_DIRS}")
    message(STATUS "Found ICU libraries: ${ICU_LIBRARIES}")
  endif()
else()
  if(ICU_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find ICU")
  else()
    message(STATUS "Optional package ICU was not found")
    endif()
endif()
