# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# Write a "#error" block if the user has defined a macro that is no longer supported
function(libical_removed_macros option)
  file(APPEND ${ICAL_FILE_H_FILE} "#if defined(${option})\n")
  file(APPEND ${ICAL_FILE_H_FILE} "#error You are specifying the ${option} macro which is no longer supported\n")
  file(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
endfunction()

# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header ical.h properly.
set(
  COMBINEDHEADERSICAL
  ${TOPB}/src/libical/icalversion.h
  ${TOPB}/src/libical/icaltime.h
  ${TOPS}/src/libical/icalduration.h
  ${TOPS}/src/libical/icalperiod.h
  ${TOPS}/src/libical/icalenums.h
  ${TOPS}/src/libical/icaltypes.h
  ${TOPS}/src/libical/icalarray.h
  ${TOPS}/src/libical/icalenumarray.h
  ${TOPS}/src/libical/icalstrarray.h
  ${TOPS}/src/libical/icalrecur.h
  ${TOPS}/src/libical/icalattach.h
  ${TOPB}/src/libical/icalderivedvalue.h
  ${TOPB}/src/libical/icalderivedparameter.h
  ${TOPS}/src/libical/icalvalue.h
  ${TOPS}/src/libical/icalparameter.h
  ${TOPB}/src/libical/icalderivedproperty.h
  ${TOPS}/src/libical/icalpvl.h
  ${TOPS}/src/libical/icalproperty.h
  ${TOPS}/src/libical/icalcomponent.h
  ${TOPS}/src/libical/icaltimezone.h
  ${TOPS}/src/libical/icalparser.h
  ${TOPS}/src/libical/icalmemory.h
  ${TOPS}/src/libical/icalerror.h
  ${TOPS}/src/libical/icallimits.h
  ${TOPS}/src/libical/icalrestriction.h
  ${TOPS}/src/libical/icallangbind.h
)

file(WRITE ${ICAL_FILE_H_FILE} "#ifndef LIBICAL_ICAL_H\n")
file(APPEND ${ICAL_FILE_H_FILE} "#define LIBICAL_ICAL_H\n")
file(APPEND ${ICAL_FILE_H_FILE} "#ifndef S_SPLINT_S\n")
file(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
file(APPEND ${ICAL_FILE_H_FILE} "extern \"C\" {\n")
file(APPEND ${ICAL_FILE_H_FILE} "#endif\n")

libical_removed_macros(ICAL_ENABLE_ERRORS_ARE_FATAL)
libical_removed_macros(ICAL_ALLOW_EMPTY_PROPERTIES)
libical_removed_macros(PVL_USE_MACROS)
libical_removed_macros(ICAL_SETERROR_ISFUNC)

foreach(_current_FILE ${COMBINEDHEADERSICAL})
  file(STRINGS ${_current_FILE} _lines NEWLINE_CONSUME)
  foreach(_currentLINE ${_lines})
    # do not remove includes for private headers (of the form ical*_p.h)
    string(REGEX REPLACE "#include \"ical.*[^_p]\\.h\"" "" _currentLINE "${_currentLINE}")
    string(REGEX REPLACE "#include \"config.*\\.h\"" "" _currentLINE "${_currentLINE}")
    if(NOT "${_currentLINE}" STREQUAL "")
      file(APPEND ${ICAL_FILE_H_FILE} "${_currentLINE}\n")
    endif()
  endforeach()
endforeach()

file(APPEND ${ICAL_FILE_H_FILE} "\n")
file(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
file(APPEND ${ICAL_FILE_H_FILE} "}\n")
file(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
file(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
file(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
