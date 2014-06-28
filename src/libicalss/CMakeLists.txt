include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libicalss ${CMAKE_BINARY_DIR}/src/libicalss 
  ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_BINARY_DIR}/src/libical
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
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
  COMMAND
  ${CMAKE_COMMAND}
    -DTOPS:FILEPATH=${TOPS}
    -DTOPB:FILEPATH=${TOPB}
    -DICAL_FILE_H_FILE:FILEPATH=${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
    -P ${CMAKE_CURRENT_SOURCE_DIR}/icalss_file.cmake
  DEPENDS
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedproperty.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedparameter.h
  ${CMAKE_BINARY_DIR}/src/libical/icalderivedvalue.h
)

add_custom_target(icalss-header DEPENDS
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
)

########### next target ###############

#these are generated sources, but we keep them in the repo
set(icalss_LIB_DEVSRCS icalsslexer.c icalssyacc.c)

set(icalss_LIB_SRCS
   icalcalendar.c
   icalcalendar.h
   icalclassify.c
   icalclassify.h
   icalcluster.c
   icalcluster.h
   icalclusterimpl.h
   icalgauge.c
   icalgauge.h
   icalgaugeimpl.h
   icaldirset.c
   icaldirset.h
   icaldirsetimpl.h
   icalfileset.c
   icalfileset.h
   icalfilesetimpl.h
   icalset.c
   icalset.h
   icalssyacc.h
   icalspanlist.c
   icalspanlist.h
   icalmessage.c
   icalmessage.h
#   $(BDB_SOURCEFILES)
   ${icalss_LIB_DEVSRCS}
)

if(MSVC)
  list(APPEND icalss_LIB_SRCS ../icalss.def)
endif(MSVC)

add_library(icalss ${LIBRARY_TYPE} ${icalss_LIB_SRCS})
add_library(icalss-static STATIC ${icalss_LIB_SRCS})

add_dependencies(icalss icalss-header)

target_link_libraries(icalss ical)

if(MSVC)
  set_target_properties(icalss PROPERTIES OUTPUT_NAME "libicalss")
  set_target_properties(icalss-static PROPERTIES OUTPUT_NAME "libicalss-static")
else(MSVC)
  set_target_properties(icalss-static PROPERTIES OUTPUT_NAME "icalss")
endif(MSVC)
set_target_properties(icalss PROPERTIES VERSION ${LIBICAL_LIB_VERSION_STRING} SOVERSION ${LIBICAL_LIB_MAJOR_VERSION})
set_target_properties(icalss PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(icalss-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS icalss icalss-static ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
  ${CMAKE_BINARY_DIR}/src/libicalss/icalss.h
  icalcalendar.h
  icalclassify.h
  icalcluster.h
  icaldirset.h 
  icaldirsetimpl.h
  icalfileset.h
  icalfilesetimpl.h
  icalgauge.h
  icalgaugeimpl.h
  icalmessage.h
  icalset.h
  icalspanlist.h
  icalssyacc.h
  #$(libicalssinclude_BDBHEADERS)
  DESTINATION
 ${INCLUDE_INSTALL_DIR}/libical
)

#original Makefile.am contents follow:

##======================================================================
##  FILE: Makefile.am
##  CREATOR: eric 
##  
##  $Id: Makefile.am,v 1.24 2008-02-03 15:27:34 dothebart Exp $
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
#AM_YFLAGS =-d -v -p ss
#AM_LFLAGS = -Pss 
#LEX_OUTPUT_ROOT = lex.ss
#
#all: icalss.h
#
## just to get it built
#$(srcdir)/icalgauge.c: icalssyacc.h
#$(srcdir)/icalsslexer.c: icalssyacc.h 
#
#
#if WITH_BDB4
#BDB_INCLUDE=-I@BDB_DIR_INCLUDE@ -DWITH_BDB4
#BDB_SOURCEFILES=icalbdbset.c icalbdbset.h icalbdbsetimpl.h
#BDB_HEADERFILES=$(srcdir)/icalbdbset.h
#libicalssinclude_BDBHEADERS=icalbdbset.h icalbdbsetimpl.h
#BDB_LIBFILES=@BDB_DIR_LIB@/@BDB_LIB@
#else
#BDB_INCLUDE=
#BDB_SOURCEFILES=
#BDB_HEADERFILES=
#endif
#
#if WITH_CXX_BINDINGS
#cxx_lib=libicalss_cxx.la
#cxx_headers=icalspanlist_cxx.h
#else
#cxx_lib=
#cxx_headers=
#endif
#
#lib_LTLIBRARIES = libicalss.la $(cxx_lib)
#
#INCLUDES =			\
#	-I$(top_srcdir)/src	\
#	-I$(top_builddir)/src	\
#	-I$(top_srcdir)/src/libical	\
#	-I$(top_builddir)/src/libical	\
#	$(BDB_INCLUDE)
#
#libicalss_la_LDFLAGS = -version-info 48:0:48
#libicalss_la_LIBADD = $(BDB_LIBFILES) 
#
#if DEV
#libicalss_la_DEVSOURCES = icalsslexer.l icalssyacc.y
#else
#libicalss_la_DEVSOURCES = icalsslexer.c icalssyacc.c
#endif
#
#libicalss_la_SOURCES =		\
#	icalcalendar.c		\
#	icalcalendar.h		\
#	icalclassify.c		\
#	icalclassify.h		\
#	icalcluster.c		\
#	icalcluster.h		\
#	icalclusterimpl.h	\
#	icalgauge.c		\
#	icalgauge.h		\
#	icalgaugeimpl.h		\
#	icaldirset.c		\
#	icaldirset.h		\
#	icaldirsetimpl.h	\
#	icalfileset.c		\
#	icalfileset.h		\
#	icalfilesetimpl.h	\
#	icalset.c		\
#	icalset.h		\
#	icalssyacc.h		\
#	icalspanlist.c		\
#	icalspanlist.h		\
#	icalmessage.c		\
#	icalmessage.h		\
#	$(BDB_SOURCEFILES)	\
#	$(libicalss_la_DEVSOURCES)
#
#libicalssincludedir = $(includedir)/libical
#
#COMBINEDHEADERS = \
#        $(srcdir)/icalgauge.h \
#        $(srcdir)/icalset.h \
#        $(srcdir)/icalcluster.h \
#        $(srcdir)/icalfileset.h \
#        $(srcdir)/icaldirset.h \
#	$(BDB_HEADERFILES) \
#        $(srcdir)/icalcalendar.h  \
#        $(srcdir)/icalclassify.h \
#	$(srcdir)/icalspanlist.h	\
#	$(srcdir)/icalmessage.h
#
#if WITH_CXX_BINDINGS
#libicalss_cxx_la_SOURCES =	\
#	$(libicalss_evolution_la_SOURCES)	\
#	icalspanlist_cxx.h	\
#	icalspanlist_cxx.cpp
#
#endif
#
#icalss.h: $(COMBINEDHEADERS)
#	echo '#ifndef LIBICAL_ICALSS_H'   >  icalss.h
#	echo '#define LIBICAL_ICALSS_H'   >> icalss.h
#	echo '#ifdef __cplusplus'         >> icalss.h
#	echo 'extern "C" {'               >> icalss.h
#	echo '#endif'                     >> icalss.h
#	echo '/*'                         >> icalss.h
#	echo ' $$''Id''$$'                >> icalss.h
#	echo '*/'                         >> icalss.h
#	cat $(COMBINEDHEADERS)				\
#	| egrep -v "#include.*\"ical"			\
#	| egrep -v "#include.*\"pvl\.h\""		\
#	| egrep -v '\$$(Id|Locker): .+\$$'>> icalss.h
#	echo '#ifdef __cplusplus'         >> icalss.h
#	echo '}'                          >> icalss.h
#	echo '#endif'                     >> icalss.h
#	echo '#endif'                     >> icalss.h
#
#libicalssinclude_HEADERS = icalss.h  icalcalendar.h icalclassify.h icalcluster.h icaldirset.h icaldirsetimpl.h icalfileset.h icalfilesetimpl.h icalgauge.h icalgaugeimpl.h icalmessage.h icalset.h icalspanlist.h icalssyacc.h $(libicalssinclude_BDBHEADERS)
#
#CONFIG_CLEAN_FILES = y.output
#
