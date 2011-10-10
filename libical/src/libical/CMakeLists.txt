configure_file(${CMAKE_CURRENT_SOURCE_DIR}/icalversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/icalversion.h @ONLY)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libical
  ${CMAKE_BINARY_DIR}/src/libical
)

if(WIN32)
  if(MSVC)
    add_definitions(-DBUILD_LIBICALDLL)
  endif(MSVC)
endif(WIN32)

configure_file(icalerror.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/icalerror.h)

set(PACKAGE_DATA_DIR "\\\"${CMAKE_INSTALL_PREFIX}/share/libical\\\"")
add_definitions(-DPACKAGE_DATA_DIR="${PACKAGE_DATA_DIR}")



set(ICALSCRIPTS ${CMAKE_SOURCE_DIR}/scripts/ )

set(PROPERTYDEPS
  ${ICALSCRIPTS}/mkderivedproperties.pl
  ${CMAKE_SOURCE_DIR}/design-data/properties.csv
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
)


add_custom_command(
  OUTPUT 
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/properties.csv
    ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  DEPENDS ${PROPERTYDEPS}
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
  COMMAND
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedproperties.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedproperty.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/properties.csv
    ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.c
    DEPENDS ${PROPERTYDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
)

set(PARAMETERDEPS
  ${ICALSCRIPTS}/mkderivedparameters.pl
  ${CMAKE_SOURCE_DIR}/design-data/parameters.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
)

add_custom_command(
  OUTPUT 
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  COMMAND 
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/parameters.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  DEPENDS ${PARAMETERDEPS}
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedparameters.pl 
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedparameter.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/parameters.csv > 
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.c
  DEPENDS ${PARAMETERDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
)

set(RESTRICTIONDEPS
  ${ICALSCRIPTS}/mkrestrictiontable.pl
  ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src/libical/)
add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  COMMAND
  ${PERL_EXECUTABLE} -I ${ICALSCRIPTS} ${ICALSCRIPTS}/mkrestrictiontable.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalrestriction.c.in
    ${CMAKE_SOURCE_DIR}/design-data/restrictions.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalrestriction.c
  DEPENDS ${RESTRICTIONDEPS}
)

set(VALUEDEPS
  ${ICALSCRIPTS}mkderivedvalues.pl
  ${CMAKE_SOURCE_DIR}/design-data/value-types.csv
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
  ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  COMMAND
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl 
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.h.in
    -h ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  DEPENDS ${VALUEDEPS}
)

add_custom_command(
  OUTPUT 
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  COMMAND 
  ${PERL_EXECUTABLE} -I${ICALSCRIPTS} ${ICALSCRIPTS}/mkderivedvalues.pl
    -i ${CMAKE_SOURCE_DIR}/src/libical/icalderivedvalue.c.in
    -c ${CMAKE_SOURCE_DIR}/design-data/value-types.csv >
    ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.c
  DEPENDS ${VALUEDEPS} ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

if(WIN32)
  set(TOPS "\"${CMAKE_SOURCE_DIR}\"")
  set(TOPB "\"${CMAKE_BINARY_DIR}\"")
else(WIN32)
  set(TOPS "${CMAKE_SOURCE_DIR}")
  set(TOPB "${CMAKE_BINARY_DIR}")
endif(WIN32)

add_custom_command(
  OUTPUT
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
  COMMAND
  ${CMAKE_COMMAND}
    -DTOPS:FILEPATH=${TOPS}
    -DTOPB:FILEPATH=${TOPB}
    -DICAL_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libical/ical.h
    -P ${CMAKE_CURRENT_SOURCE_DIR}/ical_file.cmake
  DEPENDS
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

add_custom_target(ical-header DEPENDS
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
)

########### next target ###############

set(BUILT_COMBINEDHEADERS
   ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
   ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
   ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

set(BUILT_SOURCES
   ${BUILT_COMBINEDHEADERS}
   icalderivedparameter.c
   icalderivedproperty.c
   icalrestriction.c
   icalderivedvalue.c
)

SET(ical_LIB_SRCS
   ${BUILT_SOURCES}
   icalarray.c
   icalarray.h
   icalattach.h
   icalattachimpl.h
   icalattach.c
   icalcomponent.c
   icalcomponent.h
   icalenums.c
   icalenums.h
   icalerror.c
   icalmemory.c
   icalmemory.h
   icalmime.c
   icalmime.h
   icalparameter.c
   icalparameter.h
   icalparameterimpl.h
   icalparser.c
   icalparser.h
   icalproperty.c
   icalproperty.h
   icalrecur.c
   icalrecur.h
   icalrestriction.h
   icaltime.c
   icaltime.h
   icaltz-util.c
   icaltz-util.h
   icaltimezone.c
   icaltimezone.h
   icalduration.h
   icalduration.c
   icalperiod.h
   icalperiod.c
   icaltypes.c
   icaltypes.h
   icalvalue.c
   icalvalue.h
   icalvalueimpl.h
   pvl.c
   pvl.h
   sspm.c
   sspm.h
   icallangbind.h
   icallangbind.c
   caldate.c
   astime.h
)
if(WIN32)
  if(MSVC)
    list(APPEND ical_LIB_SRCS ../ical.def)
  endif(MSVC)
endif(WIN32)

add_library(ical ${LIBRARY_TYPE} ${ical_LIB_SRCS})
add_library(ical-static STATIC ${ical_LIB_SRCS})

add_dependencies(ical ical-header)

target_link_libraries(ical ${CMAKE_THREAD_LIBS_INIT})

if (WINCE)
  target_link_libraries(ical ${WCECOMPAT_LIBRARIES})
endif(WINCE)

if(MSVC)
  set_target_properties(ical PROPERTIES OUTPUT_NAME "libical")
  set_target_properties(ical-static PROPERTIES OUTPUT_NAME "libical-static")
else(MSVC)
  set_target_properties(ical-static PROPERTIES OUTPUT_NAME "ical")
endif(MSVC)
set_target_properties(ical PROPERTIES VERSION ${LIBICAL_LIB_VERSION_STRING} SOVERSION ${LIBICAL_LIB_MAJOR_VERSION})
set_target_properties(ical PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ical-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS ical ical-static ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES ${CMAKE_SOURCE_DIR}/src/ical.h DESTINATION ${INCLUDE_INSTALL_DIR})

install(FILES
  ${CMAKE_BINARY_DIR}/src/libical/ical.h
  icalarray.h
  icalattach.h
  icalcomponent.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
  icalduration.h
  icalenums.h
  ${CMAKE_BINARY_DIR}/src/libical/icalerror.h
  icallangbind.h
  icalmemory.h
  icalmime.h
  icalparameter.h
  icalparser.h
  icalperiod.h
  icalproperty.h
  icalrecur.h
  icalrestriction.h
  icaltime.h
  icaltz-util.h
  icaltimezone.h
  icaltypes.h
  icalvalue.h
  pvl.h
  sspm.h
  #$(cxx_headers)
  DESTINATION 
  ${INCLUDE_INSTALL_DIR}/libical)


#original Makefile.am contents follow:

##======================================================================
##  FILE: Makefile.am
##  CREATOR: eric 
##  
##  $Id: Makefile.am,v 1.40 2008-01-02 20:07:30 dothebart Exp $
##
##
## (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of either: 
##
##    The LGPL as published by the Free Software Foundation, version
##    2.1, available at: http://www.fsf.org/copyleft/lesser.html
##
##  Or:
##
##    The Mozilla Public License Version 1.0. You may obtain a copy of
##    the License at http://www.mozilla.org/MPL/
##
##  
##
##======================================================================
#
#
#DESIGNDATA = $(top_srcdir)/design-data
#ICALSCRIPTS = $(top_srcdir)/scripts
#
#if WITH_CXX_BINDINGS
#cxx_lib=libical_cxx.la
#cxx_headers=vcomponent.h icalparameter_cxx.h icalproperty_cxx.h icalvalue_cxx.h icptrholder.h
#else
#cxx_lib=
#cxx_headers=
#endif
#
#lib_LTLIBRARIES = libical.la $(cxx_lib)
#noinst_LTLIBRARIES = libical-static.la
#libical_static_la_SOURCES = $(libical_la_SOURCES)
#libical_static_la_LDFLAGS = --all-static
#
#AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\""$(datadir)/$(PACKAGE)"\"
#
## CFLAGS = -g
#
#INCLUDES =				\
#	-I$(top_builddir)		\
#	-I$(top_srcdir)/src		\
#	-I$(top_builddir)/src		\
#	-I$(top_srcdir)/src/libical	\
#	-I$(top_builddir)/src/libical	\
#	-I$(srcdir)
#
#libical_la_LDFLAGS = -version-info 42:0:42
#
#libical_la_LIBADD = $(PTHREAD_LIBS)
#
#libical_la_SOURCES =		\
#	$(BUILT_SOURCES)	\
#	icalarray.c		\
#	icalarray.h		\
#	icalattach.h		\
#	icalattachimpl.h	\
#	icalattach.c		\
#	icalcomponent.c		\
#	icalcomponent.h		\
#	icalenums.c		\
#	icalenums.h		\
#	icalerror.c		\
#	icalerror.h		\
#	icalmemory.c		\
#	icalmemory.h		\
#	icalmime.c		\
#	icalmime.h		\
#	icalparameter.c		\
#	icalparameter.h		\
#	icalparameterimpl.h	\
#	icalparser.c		\
#	icalparser.h		\
#	icalproperty.c		\
#	icalproperty.h		\
#	icalrecur.c		\
#	icalrecur.h		\
#	icalrestriction.h	\
#	icaltime.c		\
#	icaltime.h		\
#	icaltz-util.c		\
#	icaltz-util.h		\
#	icaltimezone.c		\
#	icaltimezone.h		\
#	icalduration.h		\
#	icalduration.c		\
#	icalperiod.h		\
#	icalperiod.c		\
#	icaltypes.c		\
#	icaltypes.h		\
#	icalvalue.c		\
#	icalvalue.h		\
#	icalvalueimpl.h		\
#	pvl.c			\
#	pvl.h			\
#	sspm.c			\
#	sspm.h			\
#	vsnprintf.c		\
#	icallangbind.h		\
#	icallangbind.c \
#	caldate.c \
#	astime.h 
#
#if WITH_CXX_BINDINGS
#libical_cxx_la_SOURCES =	\
#	$(libical_la_SOURCES)	\
#	vcomponent.h		\
#	vcomponent.cpp		\
#	icalproperty_cxx.h	\
#	icalproperty_cxx.cpp	\
#	icalparameter_cxx.h	\
#	icalparameter_cxx.cpp	\
#	icalvalue_cxx.h		\
#	icalvalue_cxx.cpp       \
#	icptrholder.h
#endif
#
#libical_includedir = $(includedir)/libical
#
#libical_include_HEADERS = \
#	ical.h			\
#	icalarray.h		\
#	icalattach.h		\
#	icalcomponent.h		\
#	icalderivedparameter.h	\
#	icalderivedproperty.h	\
#	icalderivedvalue.h	\
#	icalduration.h		\
#	icalenums.h		\
#	icalerror.h		\
#	icallangbind.h		\
#	icalmemory.h		\
#	icalmime.h		\
#	icalparameter.h		\
#	icalparser.h		\
#	icalperiod.h		\
#	icalproperty.h		\
#	icalrecur.h		\
#	icalrestriction.h	\
#	icaltime.h		\
#	icaltz-util.h		\
#	icaltimezone.h		\
#	icaltypes.h		\
#	icalvalue.h		\
#	pvl.h			\
#	sspm.h			\
#	$(cxx_headers)
#
## ORDERING OF HEADERS IS SIGNIFICANT. Don't change this ordering. It
## is required to make the combined header ical.h properly
#COMBINEDHEADERS =					\
#	$(top_builddir)/src/libical/icalversion.h	\
#	$(top_srcdir)/src/libical/icaltime.h		\
#	$(top_srcdir)/src/libical/icalduration.h	\
#	$(top_srcdir)/src/libical/icalperiod.h		\
#	$(top_srcdir)/src/libical/icalenums.h		\
#	$(top_srcdir)/src/libical/icaltypes.h		\
#	$(top_srcdir)/src/libical/icalrecur.h		\
#	$(top_srcdir)/src/libical/icalattach.h		\
#	icalderivedvalue.h				\
#	icalderivedparameter.h				\
#	$(top_srcdir)/src/libical/icalvalue.h		\
#	$(top_srcdir)/src/libical/icalparameter.h	\
#	icalderivedproperty.h				\
#	$(top_srcdir)/src/libical/icalproperty.h	\
#	$(top_srcdir)/src/libical/pvl.h			\
#	$(top_srcdir)/src/libical/icalarray.h		\
#	$(top_srcdir)/src/libical/icalcomponent.h	\
#	$(top_srcdir)/src/libical/icaltimezone.h	\
#	$(top_srcdir)/src/libical/icalparser.h		\
#	$(top_srcdir)/src/libical/icalmemory.h		\
#	$(top_srcdir)/src/libical/icalerror.h		\
#	$(top_srcdir)/src/libical/icalrestriction.h	\
#	$(top_srcdir)/src/libical/sspm.h		\
#	$(top_srcdir)/src/libical/icalmime.h 		\
#	$(top_srcdir)/src/libical/icallangbind.h	
#
## exporting the configure stuff is probably no good idea. 
## others use standard defines not in our namespace too.
##        $(top_builddir)/config.h                        
#
#BUILT_COMBINEDHEADERS = 	\
#	icalderivedparameter.h	\
#	icalderivedproperty.h	\
#	icalderivedvalue.h
#
#BUILT_SOURCES =			\
#	$(BUILT_COMBINEDHEADERS)\
#	icalderivedparameter.c	\
#	icalderivedproperty.c	\
#	icalrestriction.c	\
#	icalderivedvalue.c
#
#ical.h: $(COMBINEDHEADERS)
#	echo '#ifndef LIBICAL_ICAL_H'     >  ical.h
#	echo '#define LIBICAL_ICAL_H'     >> ical.h
#	echo '#ifdef __cplusplus'         >> ical.h
#	echo 'extern "C" {'               >> ical.h
#	echo '#endif'                     >> ical.h
#	echo '/*'                         >> ical.h
#	echo ' $$''Id''$$'                >> ical.h
#	echo '*/'                         >> ical.h
#	cat $(COMBINEDHEADERS)				\
#	| egrep -v "#include.*\"ical"			\
#	| egrep -v "#include.*\"config"			\
#	| egrep -v "#include.*\"pvl\.h\""		\
#	| egrep -v '\$$(Id|Locker): .+\$$'>> ical.h
#	echo '#ifdef __cplusplus'         >> ical.h
#	echo '}'                          >> ical.h
#	echo '#endif'                     >> ical.h
#	echo '#endif'                     >> ical.h
#
#
## parameters
#
#PARAMETERDEPS =	\
#	$(ICALSCRIPTS)/mkderivedparameters.pl \
#	$(DESIGNDATA)/parameters.csv	\
#	icalderivedparameter.c.in \
#	icalderivedparameter.h.in
#
#icalderivedparameter.h: $(PARAMETERDEPS) 
#	$(PERL) -I $(ICALSCRIPTS) $(ICALSCRIPTS)/mkderivedparameters.pl -i $(srcdir)/icalderivedparameter.h.in -h $(DESIGNDATA)/parameters.csv > icalderivedparameter.h 
#
#icalderivedparameter.c: $(PARAMETERDEPS) $(BUILT_COMBINEDHEADERS)
#	$(PERL) -I $(ICALSCRIPTS) $(ICALSCRIPTS)/mkderivedparameters.pl -i $(srcdir)/icalderivedparameter.c.in -c $(DESIGNDATA)/parameters.csv > icalderivedparameter.c
#
## properties
#
#PROPERTYDEPS =					\
#	$(ICALSCRIPTS)/mkderivedproperties.pl	\
#	$(DESIGNDATA)/properties.csv		\
#	$(DESIGNDATA)/value-types.csv		\
#	icalderivedproperty.c.in		\
#	icalderivedproperty.h.in
#
#
#icalderivedproperty.h: $(PROPERTYDEPS)
#	$(PERL)  -I$(ICALSCRIPTS) $(ICALSCRIPTS)/mkderivedproperties.pl \
#		-i $(srcdir)/icalderivedproperty.h.in -h $(DESIGNDATA)/properties.csv\
#		${DESIGNDATA}/value-types.csv > icalderivedproperty.h 
#
#icalderivedproperty.c: $(PROPERTYDEPS) $(BUILT_COMBINEDHEADERS)
#	$(PERL)  -I$(ICALSCRIPTS) $(ICALSCRIPTS)/mkderivedproperties.pl \
#		-i $(srcdir)/icalderivedproperty.c.in -c $(DESIGNDATA)/properties.csv \
#		${DESIGNDATA}/value-types.csv > icalderivedproperty.c
#
## restrictions
#
#RESTRICTIONDEPS =				\
#	$(ICALSCRIPTS)/mkrestrictiontable.pl	\
#	$(DESIGNDATA)/restrictions.csv		\
#	icalrestriction.c.in
#
#icalrestriction.c: $(RESTRICTIONDEPS)
#	$(PERL) $(ICALSCRIPTS)/mkrestrictiontable.pl  -i $(srcdir)/icalrestriction.c.in \
#		$(DESIGNDATA)/restrictions.csv > icalrestriction.c
#
## values
#
#VALUEDEPS =					\
#	$(ICALSCRIPTS)/mkderivedvalues.pl  	\
#	$(DESIGNDATA)/value-types.csv		\
#	icalderivedvalue.c.in				\
#	icalderivedvalue.h.in
#
#icalderivedvalue.h: $(VALUEDEPS)
#	$(PERL) -I$(ICALSCRIPTS)  $(ICALSCRIPTS)/mkderivedvalues.pl \
#	         -i $(srcdir)/icalderivedvalue.h.in -h $(DESIGNDATA)/value-types.csv > icalderivedvalue.h
#
#icalderivedvalue.c: $(VALUEDEPS) $(BUILT_COMBINEDHEADERS)
#	$(PERL)  -I$(ICALSCRIPTS) $(ICALSCRIPTS)/mkderivedvalues.pl \
#		-i $(srcdir)/icalderivedvalue.c.in -c $(DESIGNDATA)/value-types.csv > icalderivedvalue.c
#
## c++ needs ical.h auto-generated.
#icalparameter_cxx.lo: ical.h
#icalproperty_cxx.lo: ical.h
#icalvalue_cxx.lo: ical.h
#vcomponent.lo: ical.h
#
#
## housekeeping
#CONFIG_CLEAN_FILES = y.output
#
#CLEANFILES = $(BUILT_SOURCES) $(BUILT_COMBINEDHEADERS)
#
#dist-hook:
#	cd $(distdir); rm -f $(BUILT_SOURCES)
#	cd $(distdir); rm -f ical.h icalversion.h 
#
#EXTRA_DIST =			\
#	icalderivedparameter.c.in \
#	icalderivedparameter.h.in \
#	icalderivedproperty.c.in \
#	icalderivedproperty.h.in \
#	icalrestriction.c.in	\
#	icalderivedvalue.c.in	\
#	icalderivedvalue.h.in	\
#	icalversion.h.in
#
#$(srcdir)/icalcomponent.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#icalderivedparameter.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#icalderivedproperty.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#icalderivedvalue.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalduration.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icallangbind.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalmime.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalparameter.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalparser.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalproperty.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#icalrestriction.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icaltime.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
#$(srcdir)/icalvalue.c: icalderivedvalue.h icalderivedproperty.h icalderivedparameter.h
