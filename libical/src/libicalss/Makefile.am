lib_LTLIBRARIES = libicalss.la

AM_YFLAGS =-d -v -p ss
AM_LFLAGS = -Pss 
LEX_OUTPUT_ROOT = lex.ss

all: icalss.h

INCLUDES =			\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/libical \
	-I@BDB_DIR@/include

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
	icalbdbset.c		\
	icalbdbsetimpl.h	\
	icalset.c		\
	icalset.h		\
	icalssyacc.h		\
	icalspanlist.c		\
	icalspanlist.h		\
	icalmessage.c		\
	icalmessage.h		\
	$(libicalss_la_DEVSOURCES)

libicalssincludedir = $(includedir)

COMBINEDHEADERS = \
        $(srcdir)/icalgauge.h \
        $(srcdir)/icalset.h \
        $(srcdir)/icalcluster.h \
        $(srcdir)/icalfileset.h \
        $(srcdir)/icaldirset.h \
        $(srcdir)/icalbdbset.h \
        $(srcdir)/icalcalendar.h  \
        $(srcdir)/icalclassify.h \
	$(srcdir)/icalspanlist.h	\
	$(srcdir)/icalmessage.h


icalss.h: $(COMBINEDHEADERS)
	echo '#ifdef __cplusplus'         >  icalss.h
	echo 'extern "C" {'               >> icalss.h
	echo '#endif'                     >> icalss.h
	cat $(COMBINEDHEADERS) | egrep -v "#include.*\"ical" \
	| egrep -v "#include.*\"pvl\.h\"" >> icalss.h
	echo '#ifdef __cplusplus'         >> ical.h
	echo '};'                         >> ical.h
	echo '#endif'                     >> ical.h


libicalssinclude_HEADERS = icalss.h icalbdbset.h icalbdbsetimpl.h icalcalendar.h icalclassify.h icalcluster.h icaldirset.h icaldirsetimpl.h icalfileset.h icalfilesetimpl.h icalgauge.h icalgaugeimpl.h icalmessage.h icalset.h icalspanlist.h icalssyacc.h 

CONFIG_CLEAN_FILES = y.output

