lib_LTLIBRARIES = libicalss.la

YFLAGS =-d -v -p ss
LFLAGS = -Pss 
LEX_OUTPUT_ROOT = lex.ss

all: icalss.h

INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(srcdir)/libical

libicalss_la_LDFLAGS = -version-info 0:0:0

libicalss_la_SOURCES =		\
	icalclassify.c		\
	icalgauge.c		\
	icalgaugeimpl.h		\
	icaldirset.c		\
	icaldirsetimpl.h	\
	icalfileset.c		\
	icalfilesetimpl.h	\
	icalset.c		\
	icalsslexer.l		\
	icalssutil.c		\
	icalssyacc.h		\
	icalssyacc.y		\
	icalspanlist.c		\
	icalmessage.c		\
	icalcstp.c

libicalssincludedir = $(includedir)

COMBINEDHEADERS = \
        icalgauge.h \
        icalset.h \
        icalfileset.h \
        icaldirset.h \
        icalcalendar.h  \
        icalssutil.h \
        icalclassify.h \
	icalspanlist.h	\
	icalmessage.h \
	icalcstp.h

icalss.h: $(COMBINEDHEADERS)
	cat $(COMBINEDHEADERS) | egrep -v "#include.*\"ical" \
	| egrep -v "#include.*\"pvl\.h\"" > icalss.h


libicalssinclude_HEADERS = icalss.h

CONFIG_CLEAN_FILES = y.output

EXTRA_DIST =			\
	$(COMBINEDHEADERS)	\
