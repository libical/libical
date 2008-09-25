# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header icalss.h properly.
set(COMBINEDHEADERSICALSS
   ${TOPS}/src/libicalss/icalclassify.h
)

FILE(WRITE  ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "extern \"C\" {\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")

foreach (_current_FILE ${COMBINEDHEADERSICALSS})
   FILE(READ ${_current_FILE} _contents)
   STRING(REGEX REPLACE "#include *\"ical.*\\.h\"" "" _contents "${_contents}" )
   STRING(REGEX REPLACE "#include *\"pvl\\.h\"" "" _contents "${_contents}" )
   FILE(APPEND ${ICAL_FILE_H_FILE} "${_contents}")
endforeach (_current_FILE)

FILE(APPEND ${ICAL_FILE_H_FILE} "\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "}\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
