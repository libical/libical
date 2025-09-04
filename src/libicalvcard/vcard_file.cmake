# SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header vcard.h properly.
set(
  COMBINEDHEADERSVCARD
  ${TOPS}/src/libicalvcard/vcardenumarray.h
  ${TOPS}/src/libicalvcard/vcardstrarray.h
  ${TOPS}/src/libicalvcard/vcardstructured.h
  ${TOPS}/src/libicalvcard/vcardtime.h
  ${TOPS}/src/libicalvcard/vcardtypes.h
  ${TOPB}/src/libicalvcard/vcardderivedvalue.h
  ${TOPB}/src/libicalvcard/vcardderivedparameter.h
  ${TOPS}/src/libicalvcard/vcardvalue.h
  ${TOPS}/src/libicalvcard/vcardparameter.h
  ${TOPB}/src/libicalvcard/vcardderivedproperty.h
  ${TOPS}/src/libicalvcard/vcardproperty.h
  ${TOPS}/src/libicalvcard/vcardcomponent.h
  ${TOPS}/src/libicalvcard/vcardparser.h
  ${TOPS}/src/libicalvcard/vcardrestriction.h
)

file(WRITE ${VCARD_FILE_H_FILE} "#ifndef LIBICAL_VCARD_H\n")
file(APPEND ${VCARD_FILE_H_FILE} "#define LIBICAL_VCARD_H\n")
file(APPEND ${VCARD_FILE_H_FILE} "#ifndef S_SPLINT_S\n")
file(APPEND ${VCARD_FILE_H_FILE} "#ifdef __cplusplus\n")
file(APPEND ${VCARD_FILE_H_FILE} "extern \"C\" {\n")
file(APPEND ${VCARD_FILE_H_FILE} "#endif\n")

foreach(_current_FILE ${COMBINEDHEADERSVCARD})
  file(STRINGS ${_current_FILE} _lines NEWLINE_CONSUME)
  foreach(_currentLINE ${_lines})
    # do not remove includes for private headers (of the form vcard*_p.h)
    string(REGEX REPLACE "#include \"vcard.*[^_p]\\.h\"" "" _currentLINE "${_currentLINE}")
    string(REGEX REPLACE "#include \"config.*\\.h\"" "" _currentLINE "${_currentLINE}")
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
