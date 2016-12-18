# Finds the International Components for Unicode (ICU) Library
#
#  ICU_FOUND          - True if ICU found.
#  ICU_I18N_FOUND     - True if ICU's internationalization library found.
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

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(_pc_icu icu-i18n)
  if(_pc_icu_FOUND)
    set(ICU_FOUND 1)
    set(ICU_I18N_FOUND 1)
    string(REPLACE ";" " " ICU_LDFLAGS "${_pc_icu_LDFLAGS}")
    set(ICU_LIBRARIES ${ICU_LDFLAGS})
    set(ICU_LIBRARY ${ICU_LDFLAGS})
    set(ICU_I18N_LIBRARY ${ICU_LDFLAGS})
    mark_as_advanced(ICU_LIBRARY)
    mark_as_advanced(ICU_I18N_LIBRARY)
    set(ICU_INCLUDE_DIRS ${_pc_icu_CFLAGS})
    set(ICU_INCLUDE_DIR ${_pc_icu_INCLUDE_DIRS})
    mark_as_advanced(ICU_INCLUDE_DIR)
    set(ICU_VERSION ${_pc_icu_VERSION})
    string(REGEX REPLACE "([0-9]+)[.].*" "\\1" ICU_MAJOR_VERSION "${ICU_VERSION}")
    string(REGEX REPLACE ".*[.]([0-9]+)" "\\1" ICU_MINOR_VERSION "${ICU_VERSION}")
  endif()
endif()

if(NOT ICU_FOUND)

if(WIN32)
  file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _program_FILES_DIR)
endif()

if(ICU_INCLUDE_DIR AND ICU_LIBRARY)
  # Already in cache, be silent
  set(ICU_FIND_QUIETLY TRUE)
endif()

#set the root from the ICU_BASE environment
file(TO_NATIVE_PATH "$ENV{ICU_BASE}" icu_root)
#override the root from ICU_BASE defined to cmake
if(DEFINED ICU_BASE)
  file(TO_NATIVE_PATH "${ICU_BASE}" icu_root)
endif()

# Look for the header file.
find_path(
  ICU_INCLUDE_DIR
  NAMES unicode/utypes.h
  HINTS
  ${icu_root}/include
  ${_program_FILES_DIR}/icu/include
  /usr/local/opt/icu4c/include
  DOC "Include directory for the ICU library"
)
mark_as_advanced(ICU_INCLUDE_DIR)

# Look for the library.
find_library(
  ICU_LIBRARY
  NAMES icuuc cygicuuc cygicuuc32
  HINTS
  ${icu_root}/lib/
  ${_program_FILES_DIR}/icu/lib/
  /usr/local/opt/icu4c/lib/
  DOC "Libraries to link against for the common parts of ICU"
)
mark_as_advanced(ICU_LIBRARY)

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
