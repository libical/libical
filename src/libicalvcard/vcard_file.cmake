# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header ical.h properly.
set(COMBINEDHEADERSVCARD
  ${TOPB}/src/libicalvcard/vcardderivedvalue.h
  ${TOPB}/src/libicalvcard/vcardderivedparameter.h
  ${TOPS}/src/libicalvcard/vcardvalue.h
  ${TOPS}/src/libicalvcard/vcardparameter.h
  ${TOPB}/src/libicalvcard/vcardderivedproperty.h
  ${TOPS}/src/libicalvcard/vcardproperty.h
  ${TOPS}/src/libicalvcard/vcardcomponent.h
)

file(WRITE ${VCARD_FILE_H_FILE} "#ifndef LIBICALVCARD_ICAL_H\n")
file(APPEND ${VCARD_FILE_H_FILE} "#define LIBICALVCARD_ICAL_H\n")
file(APPEND ${VCARD_FILE_H_FILE} "#ifndef S_SPLINT_S\n")
file(APPEND ${VCARD_FILE_H_FILE} "#ifdef __cplusplus\n")
file(APPEND ${VCARD_FILE_H_FILE} "extern \"C\" {\n")
file(APPEND ${VCARD_FILE_H_FILE} "#endif\n")

foreach(_current_FILE ${COMBINEDHEADERSVCARD})
  file(STRINGS ${_current_FILE} _lines NEWLINE_CONSUME)
  foreach(_currentLINE ${_lines})
    string(REGEX REPLACE "#include \"vcard.*\\.h\"" "" _currentLINE "${_currentLINE}")
    string(REGEX REPLACE "#include \"config.*\\.h\"" "" _currentLINE "${_currentLINE}")
    string(REGEX REPLACE "#include \"pvl\\.h\"" "" _currentLINE "${_currentLINE}" )
    if(NOT "${_currentLINE}" STREQUAL "")
      file(APPEND ${VCARD_FILE_H_FILE} "${_currentLINE}\n")
    endif()
  endforeach()
endforeach()

file(APPEND ${VCARD_FILE_H_FILE} "\n")
file(APPEND ${VCARD_FILE_H_FILE} "#ifdef __cplusplus\n")
file(APPEND ${VCARD_FILE_H_FILE} "}\n")
file(APPEND ${VCARD_FILE_H_FILE} "#endif\n")
file(APPEND ${VCARD_FILE_H_FILE} "#endif\n")
file(APPEND ${VCARD_FILE_H_FILE} "#endif\n")
