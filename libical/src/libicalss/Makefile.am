lib_LTLIBRARIES = libicalss.la

AM_YFLAGS =-d -v -p ss
AM_LFLAGS = -Pss 
LEX_OUTPUT_ROOT = lex.ss

all: icalss.h

if WITH_BDB4
BDB_INCLUDE=-I@BDB_DIR_INCLUDE@ -DWITH_BDB4
BDB_SOURCES=icalbdbset.c icalbdbset.h icalbdbsetimpl.h
BDB_HEADERS=$(srcdir)/icalbdbset.h
libicalssinclude_BDBHEADERS=icalbdbset.h icalbdbsetimpl.h
else
BDB_INCLUDE=
BDB_SOURCES=
BDB_HEADERS=
endif

INCLUDES =			\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/libical \
	$(BDB_INCLUDE)

libicalss_la_LDFLAGS = -version-info 0:0:0

if DEV
libicalss_la_DEVSOURCES = icalsslexer.l icalssyacc.y
.SUFFIXES:
.SUFFIXES: .c .l .lo .o .obj .y
else
libicalss_la_DEVSOURCES = icalsslexer.c icalssyacc.c
.SUFFIXES:
.SUFFIXES: .c .lo .o .obj
endif

libicalss_la_SOURCES =		\
	icalcalendar.c		\
	icalcalendar.h		\
	icalclassify.c		\
	icalclassify.h		\
	icalcluster.c		\
	icalcluster.h		\
	icalgauge.c		\
	icalgauge.h		\
	icalgaugeimpl.h		\
	icaldirset.c		\
	icaldirset.h		\
	icaldirsetimpl.h	\
	icalfileset.c		\
	icalfileset.h		\
	icalfilesetimpl.h	\
	icalset.c		\
	icalset.h		\
	icalssyacc.h		\
	icalspanlist.c		\
	icalspanlist.h		\
	icalmessage.c		\
	icalmessage.h		\
	$(BDB_SOURCES)		\
	$(libicalss_la_DEVSOURCES)

libicalssincludedir = $(includedir)

COMBINEDHEADERS = \
        $(srcdir)/icalgauge.h \
        $(srcdir)/icalset.h \
        $(srcdir)/icalcluster.h \
        $(srcdir)/icalfileset.h \
        $(srcdir)/icaldirset.h \
	$(BDB_HEADERS) \
        $(srcdir)/icalcalendar.h  \
        $(srcdir)/icalclassify.h \
	$(srcdir)/icalspanlist.h	\
	$(srcdir)/icalmessage.h


icalss.h: $(COMBINEDHEADERS)
	cat $(COMBINEDHEADERS) | egrep -v "#include.*\"ical" \
	| egrep -v "#include.*\"pvl\.h\"" > icalss.h


libicalssinclude_HEADERS = icalss.h

CONFIG_CLEAN_FILES = y.output

