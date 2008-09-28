# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header icalss.h properly.
set(COMBINEDHEADERSICALSS
  ${TOPS}/src/libicalss/icalgauge.h
  ${TOPS}/src/libicalss/icalset.h
  ${TOPS}/src/libicalss/icalcluster.h
  ${TOPS}/src/libicalss/icalfileset.h
  ${TOPS}/src/libicalss/icaldirset.h
  ${TOPS}/src/libicalss/icalcalendar.h
  ${TOPS}/src/libicalss/icalclassify.h
  ${TOPS}/src/libicalss/icalspanlist.h
  ${TOPS}/src/libicalss/icalmessage.h
)

FILE(WRITE  ${ICAL_FILE_H_FILE} "#ifndef LIBICAL_ICALSS_H\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#define LIBICAL_ICALSS_H\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
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
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
