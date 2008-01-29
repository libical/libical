project(libical)

ADD_DEFINITIONS(-DPACKAGE_DATA_DIR=\\""${DATA_INSTALL_DIR}/"\\" ) # added manually

include (ConfigureChecks.cmake)
configure_file (config-libical.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-libical.h)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/libicalss/
	${CMAKE_CURRENT_BINARY_DIR}/src/libicalss/
	${CMAKE_CURRENT_SOURCE_DIR}/src/libical/
	${CMAKE_CURRENT_BINARY_DIR}/src/libical/
	${CMAKE_CURRENT_BINARY_DIR}/ # for config.h
)

set(ICALSCRIPTS ${CMAKE_SOURCE_DIR}/scripts/ )

set(PROPERTYDEPS 
	${ICALSCRIPTS}/mkderivedproperties.pl 
	${CMAKE_SOURCE_DIR}/design-data/properties.csv	
	${CMAKE_SOURCE_DIR}/design-data/value-types.csv
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in 
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in 
)

add_custom_command(OUTPUT
${CMAKE_BINARY_DIR}/icalderivedproperty.h
COMMAND ${PERL_EXECUTABLE} ARGS -I ${ICALSCRIPTS}
${ICALSCRIPTS}/mkderivedproperties.pl -i
${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
-h  ${CMAKE_SOURCE_DIR}/design-data/properties.csv ${CMAKE_SOURCE_DIR}/design-data/value-types.csv > ${CMAKE_BINARY_DIR}/icalderivedproperty.h   DEPENDS  ${PROPERTYDEPS} )


add_custom_command(OUTPUT
${CMAKE_BINARY_DIR}/icalderivedproperty.c
COMMAND ${PERL_EXECUTABLE} ARGS -I${ICALSCRIPTS}
${ICALSCRIPTS}/mkderivedproperties.pl -i
${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
-c ${CMAKE_SOURCE_DIR}/design-data/properties.csv
${CMAKE_SOURCE_DIR}/design-data/value-types.csv > ${CMAKE_BINARY_DIR}/icalderivedproperty.c   DEPENDS  ${PROPERTYDEPS} ${CMAKE_BINARY_DIR}/icalderivedproperty.h )


# files generated via add_custom_command are added automatically to the clean files, Alex
#macro_additional_clean_files( 
#                              ${CMAKE_BINARY_DIR}/icalderivedproperty.c 
#                              ${CMAKE_BINARY_DIR}/icalderivedproperty.h 
#)


set(PARAMETERDEPS 
	${ICALSCRIPTS}/mkderivedparameters.pl 
	${CMAKE_SOURCE_DIR}/design-data/parameters.csv 
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
)

add_custom_command(OUTPUT
${CMAKE_BINARY_DIR}/icalderivedparameter.h
COMMAND ${PERL_EXECUTABLE} ARGS -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in -h ${CMAKE_SOURCE_DIR}/design-data/parameters.csv > ${CMAKE_BINARY_DIR}/icalderivedparameter.h   DEPENDS  ${PARAMETERDEPS} )

add_custom_command(OUTPUT
${CMAKE_BINARY_DIR}/icalderivedparameter.c
COMMAND ${PERL_EXECUTABLE} ARGS -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in -c ${CMAKE_SOURCE_DIR}/design-data/parameters.csv > ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c   DEPENDS  ${PARAMETERDEPS} ${CMAKE_BINARY_DIR}/icalderivedparameter.h )

#files generated via add_custom_command are added automatically to the clean files, Alex
#macro_additional_clean_files( 
#	${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c 
#	${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h 
#)


set(RESTRICTIONDEPS 
	${ICALSCRIPTS}/mkrestrictiontable.pl
	${CMAKE_SOURCE_DIR}/design-data/restrictions.csv
	${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
)

add_custom_command(OUTPUT
${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
COMMAND ${PERL_EXECUTABLE} ARGS -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl -i ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv > ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c   DEPENDS  ${RESTRICTIONDEPS} )


#files generated via add_custom_command are added automatically to the clean files, Alex
#macro_additional_clean_files( ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c)

set(VALUEDEPS
	${CMAKE_SOURCE_DIR}/scripts/mkderivedvalues.pl
	${CMAKE_SOURCE_DIR}/design-data/value-types.csv
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
	${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/icalderivedvalue.h COMMAND ${PERL_EXECUTABLE} ARGS -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in -h ${CMAKE_SOURCE_DIR}/design-data/value-types.csv > ${CMAKE_BINARY_DIR}/icalderivedvalue.h   DEPENDS  ${VALUEDEPS} )


add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/icalderivedvalue.c COMMAND ${PERL_EXECUTABLE} ARGS -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in -c ${CMAKE_SOURCE_DIR}/design-data/value-types.csv > ${CMAKE_BINARY_DIR}/icalderivedvalue.c   DEPENDS  ${VALUEDEPS} ${CMAKE_BINARY_DIR}/icalderivedvalue.h )

#files generated via add_custom_command are added automatically to the clean files, Alex
#macro_additional_clean_files(
#	${CMAKE_BINARY_DIR}/icalderivedvalue.h
#	${CMAKE_BINARY_DIR}/icalderivedvalue.c
#)


# Keep absolute paths here, they are used in tests/CMakeLists.txt too.
set(libical_SRCS
   ${CMAKE_SOURCE_DIR}/src/libical/caldate.c       
   ${CMAKE_SOURCE_DIR}/src/libical/icalarray.c     
   ${CMAKE_SOURCE_DIR}/src/libical/icalattach.c        
   ${CMAKE_BINARY_DIR}/icalderivedparameter.c  
   ${CMAKE_SOURCE_DIR}/src/libical/icalrecur.c     
   ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c   
   ${CMAKE_SOURCE_DIR}/src/libical/icalcomponent.c     
   ${CMAKE_SOURCE_DIR}/src/libical/icalenums.c     
   ${CMAKE_SOURCE_DIR}/src/libical/icalerror.c     
   ${CMAKE_SOURCE_DIR}/src/libical/icalmemory.c        
   ${CMAKE_SOURCE_DIR}/src/libical/icalmime.c      
   ${CMAKE_SOURCE_DIR}/src/libical/icalparameter.c    
   ${CMAKE_SOURCE_DIR}/src/libical/icalparser.c        
   ${CMAKE_BINARY_DIR}/icalderivedproperty.c   
   ${CMAKE_SOURCE_DIR}/src/libical/icalproperty.c      
   ${CMAKE_SOURCE_DIR}/src/libical/icaltime.c      
   ${CMAKE_SOURCE_DIR}/src/libical/icalduration.c      
   ${CMAKE_SOURCE_DIR}/src/libical/icalperiod.c        
   ${CMAKE_SOURCE_DIR}/src/libical/icaltimezone.c      
   ${CMAKE_SOURCE_DIR}/src/libical/icaltypes.c     
   ${CMAKE_SOURCE_DIR}/src/libical/icalvalue.c     
   ${CMAKE_BINARY_DIR}/icalderivedvalue.c  
   ${CMAKE_SOURCE_DIR}/src/libical/pvl.c           
   ${CMAKE_SOURCE_DIR}/src/libical/sspm.c          
   ${CMAKE_SOURCE_DIR}/src/libical/vsnprintf.c     
)

set(libicalss_SRCS
   ${CMAKE_SOURCE_DIR}/src/libicalss/icalclassify.c      
   ${CMAKE_SOURCE_DIR}/src/libicalss/icalclassify.h      
)


#macro_additional_clean_files(${CMAKE_BINARY_DIR}/src/libical/ical.h)

if(WIN32)
  set(TOPS "\"${CMAKE_SOURCE_DIR}\"")
  set(TOPB "\"${CMAKE_BINARY_DIR}\"")
else(WIN32)
  set(TOPS "${CMAKE_SOURCE_DIR}")
  set(TOPB "${CMAKE_BINARY_DIR}")
endif(WIN32)
add_custom_command(OUTPUT
   ${CMAKE_BINARY_DIR}/src/libical/ical.h 
   COMMAND 
   ${CMAKE_COMMAND} 
   -DTOPS:FILEPATH=${TOPS}
   -DTOPB:FILEPATH=${TOPB}
   -DKDE_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libical/ical.h
   -P ${CMAKE_SOURCE_DIR}/ical_file.cmake
   DEPENDS
   ${CMAKE_BINARY_DIR}/icalderivedproperty.h
   ${CMAKE_BINARY_DIR}/icalderivedparameter.h
   ${CMAKE_BINARY_DIR}/icalderivedvalue.h
)

#macro_additional_clean_files(${CMAKE_BINARY_DIR}/src/libicalss/icalss.h)

add_custom_command(OUTPUT
   ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h 
   COMMAND 
   ${CMAKE_COMMAND} 
   -DTOP:FILEPATH=${TOPS}
   -DKDE_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
   -P ${CMAKE_SOURCE_DIR}/icalss_file.cmake
)


########### next target ###############


target_link_libraries(libical libicalss )

#set_target_properties(ical PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS libical 
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

# hack to force ical[ss].h creation
add_custom_target(ical-headers DEPENDS
        ${CMAKE_BINARY_DIR}/src/libical/ical.h
        ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
)
##### add_dependencies(ical ical-headers)

########### next target ###############

#add_subdirectory( src )

########### install files ###############

#### DESTINATION (${INCLUDE_INSTALL_DIR}/ical)
