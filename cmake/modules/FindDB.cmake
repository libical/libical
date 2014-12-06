# Finds the Berkeley DB Library
#
#  DB_FOUND          - True if Berkeley DB found.
#  DB_INCLUDE_DIR    - Directory to include to get Berkeley DB headers
#  DB_LIBRARY        - Library to link against for the Berkeley DB
#

if(DB_INCLUDE_DIR AND DB_LIBRARY)
  # Already in cache, be silent
  set(DB_FIND_QUIETLY TRUE)
endif()

# Look for the header file.
find_path(
  DB_INCLUDE_DIR
  NAMES db.h
  HINTS /usr/local/opt/db/include
  DOC "Include directory for the Berkeley DB library"
)
mark_as_advanced(DB_INCLUDE_DIR)

# Look for the library.
find_library(
  DB_LIBRARY
  NAMES db
  HINTS /usr/local/opt/db4/lib
  DOC "Libraries to link against for the Berkeley DB"
)
mark_as_advanced(DB_LIBRARY)

# Copy the results to the output variables.
if(DB_INCLUDE_DIR AND DB_LIBRARY)
  set(DB_FOUND 1)
else()
  set(DB_FOUND 0)
endif()

if(DB_FOUND)
  if(NOT DB_FIND_QUIETLY)
    message(STATUS "Found Berkeley DB header files in ${DB_INCLUDE_DIR}")
    message(STATUS "Found Berkeley libraries: ${DB_LIBRARY}")
  endif()
else()
  if(DB_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Berkeley DB")
  else()
    message(STATUS "Optional package Berkeley DB was not found")
  endif()
endif()
