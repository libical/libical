include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/libical )


########### next target ###############

SET(client_SRCS client.c)

add_executable(client ${client_SRCS})

target_link_libraries(client)


########### install files ###############

install(FILES 
  icalcap.h
  icalcap_impl.h
  icalcap_message_impl.h	
  icalcap_server.h
  icalcap_server_impl.h
  icalcap_session.h
  icalcap_session_impl.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/libicalcap)



#original Makefile.am contents follow:

##======================================================================
##  FILE: Makefile.am
##  CREATOR: acampi 
##  
##  $Id: Makefile.am,v 1.4 2008-01-02 20:07:33 dothebart Exp $
##
##
## (C) COPYRIGHT 2003, Andrea Campi, mailto:a.campi@inet.it
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
#lib_LTLIBRARIES = libicalcap-evolution.la
#
#INCLUDES =			\
#	-I$(top_builddir)	\
#	-I$(top_srcdir)/src	\
#	-I$(top_builddir)/src	\
#	-I$(top_srcdir)/src/libical	\
#	-I$(top_builddir)/libical	\
#	-I$(srcdir)		\
#	-DWITH_RR $(RRCAP_CFLAGS)
#
#libicalcap_evolution_la_LDFLAGS = $(RRCAP_LIBS) -version-info 48:0:48
#
#
#libicalcap_evolution_la_SOURCES =		\
#	icalcap.c		\
#	icalcap.h		\
#	icalcap_impl.h		\
#	icalcap_message.c	\
#	icalcap_message_impl.h	\
#	icalcap_rr.c		\
#	icalcap_server.c	\
#	icalcap_server.h	\
#	icalcap_server_impl.h	\
#	icalcap_session.c	\
#	icalcap_session.h	\
#	icalcap_session_impl.h	\
#	icalcap_utils.c
#
#libicalcap_evolutionincludedir = $(includedir)
#
#libicalcap_evolutioninclude_HEADERS =	\
#	icalcap.h		\
#	icalcap_impl.h		\
#	icalcap_message_impl.h	\
#	icalcap_server.h	\
#	icalcap_server_impl.h	\
#	icalcap_session.h	\
#	icalcap_session_impl.h
#
#
#noinst_PROGRAMS = client
#
#LDADD =  ../libicalss/libicalss-evolution.la ../libicalvcal/libicalvcal-evolution.la ../libical/libical-evolution.la libicalcap-evolution.la $(RRCAP_LIBS)
#
#
#client_SOURCES = client.c
#
