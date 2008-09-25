# ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering.
# It is required to make the combined header ical.h properly.
set(COMBINEDHEADERSICAL
   ${TOPB}/src/libical/icalversion.h
   ${TOPS}/src/libical/icaltime.h
   ${TOPS}/src/libical/icalduration.h
   ${TOPS}/src/libical/icalperiod.h
   ${TOPS}/src/libical/icalenums.h
   ${TOPS}/src/libical/icaltypes.h
   ${TOPS}/src/libical/icalrecur.h
   ${TOPS}/src/libical/icalattach.h
   ${TOPB}/src/libical/icalderivedvalue.h
   ${TOPB}/src/libical/icalderivedparameter.h
   ${TOPS}/src/libical/icalvalue.h
   ${TOPS}/src/libical/icalparameter.h
   ${TOPB}/src/libical/icalderivedproperty.h
   ${TOPS}/src/libical/icalproperty.h
   ${TOPS}/src/libical/pvl.h
   ${TOPS}/src/libical/icalarray.h
   ${TOPS}/src/libical/icalcomponent.h
   ${TOPS}/src/libical/icaltimezone.h
   ${TOPS}/src/libical/icalparser.h
   ${TOPS}/src/libical/icalmemory.h
   ${TOPS}/src/libical/icalerror.h
   ${TOPS}/src/libical/icalrestriction.h
   ${TOPS}/src/libical/sspm.h
   ${TOPS}/src/libical/icalmime.h
   ${TOPS}/src/libical/icallangbind.h
)

FILE(WRITE  ${ICAL_FILE_H_FILE} "#ifndef LIBICAL_ICAL_H\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#define LIBICAL_ICAL_H\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "extern \"C\" {\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")

foreach (_current_FILE ${COMBINEDHEADERSICAL})
   FILE(READ ${_current_FILE} _contents)
   STRING(REGEX REPLACE "#include *\"ical.*\\.h\"" "" _contents "${_contents}")
   STRING(REGEX REPLACE "#include *\"config.*\\.h\"" "" _contents "${_contents}")
   STRING(REGEX REPLACE "#include *\"pvl\\.h\"" "" _contents "${_contents}" )
   FILE(APPEND ${ICAL_FILE_H_FILE} "${_contents}")
endforeach (_current_FILE)

FILE(APPEND ${ICAL_FILE_H_FILE} "\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#ifdef __cplusplus\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "}\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
FILE(APPEND ${ICAL_FILE_H_FILE} "#endif\n")
