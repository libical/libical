#======================================================================
#  FILE: Makefile.am
#  CREATOR: acampi 
#  
#  $Id: Makefile.am,v 1.1 2003-01-21 00:08:12 acampi Exp $
#
#
# (C) COPYRIGHT 2003, Andrea Campi, mailto:a.campi@inet.it
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of either: 
#
#    The LGPL as published by the Free Software Foundation, version
#    2.1, available at: http://www.fsf.org/copyleft/lesser.html
#
#  Or:
#
#    The Mozilla Public License Version 1.0. You may obtain a copy of
#    the License at http://www.mozilla.org/MPL/
#
#  
#
#======================================================================


lib_LTLIBRARIES = libicalcap.la

noinst_LTLIBRARIES = libicalcap-static.la
libicalcap_static_la_SOURCES = $(libicalcap_la_SOURCES)
libicalcap_static_la_LDFLAGS = --all-static

INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/libical	\
	-I$(srcdir)		\
	-DWITH_RR $(RRCAP_CFLAGS)

libicalcap_la_LDFLAGS = $(RRCAP_LIBS) -version-info 0:0:0


libicalcap_la_SOURCES =		\
	icalcap.c		\
	icalcap.h		\
	icalcap_impl.h		\
	icalcap_message.c	\
	icalcap_message_impl.h	\
	icalcap_rr.c		\
	icalcap_server.c	\
	icalcap_server.h	\
	icalcap_server_impl.h	\
	icalcap_session.c	\
	icalcap_session.h	\
	icalcap_session_impl.h	\
	icalcap_utils.c

libicalcapincludedir = $(includedir)

libicalcapinclude_HEADERS =	\
	icalcap.h		\
	icalcap_impl.h		\
	icalcap_message_impl.h	\
	icalcap_server.h	\
	icalcap_server_impl.h	\
	icalcap_session.h	\
	icalcap_session_impl.h
