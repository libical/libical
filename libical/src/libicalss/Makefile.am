lib_LTLIBRARIES = libicalss.la

AM_YFLAGS =-d -v -p ss
AM_LFLAGS = -Pss 
LEX_OUTPUT_ROOT = lex.ss

all: icalss.h

# just to get it built
$(srcdir)/icalgauge.c: icalssyacc.h
$(srcdir)/icalsslexer.c: icalssyacc.h 


if WITH_BDB4
BDB_INCLUDE=-I@BDB_DIR_INCLUDE@ -DWITH_BDB4
BDB_SOURCEFILES=icalbdbset.c icalbdbset.h icalbdbsetimpl.h
BDB_HEADERFILES=$(srcdir)/icalbdbset.h
libicalssinclude_BDBHEADERS=icalbdbset.h icalbdbsetimpl.h
BDB_LIBFILES=@BDB_DIR_LIB@/@BDB_LIB@
else
BDB_INCLUDE=
BDB_SOURCEFILES=
BDB_HEADERFILES=
endif

INCLUDES =			\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/libical \
	$(BDB_INCLUDE)

libicalss_la_LDFLAGS = -version-info 0:0:0
libicalss_la_LDADD = $(BDB_LIBFILES)

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
	$(BDB_SOURCEFILES)	\
	$(libicalss_la_DEVSOURCES)

libicalssincludedir = $(includedir)

COMBINEDHEADERS = \
        $(srcdir)/icalgauge.h \
        $(srcdir)/icalset.h \
        $(srcdir)/icalcluster.h \
        $(srcdir)/icalfileset.h \
        $(srcdir)/icaldirset.h \
	$(BDB_HEADERFILES) \
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
	echo '#ifdef __cplusplus'         >> icalss.h
	echo '};'                         >> icalss.h
	echo '#endif'                     >> icalss.h


libicalssinclude_HEADERS = icalss.h  icalcalendar.h icalclassify.h icalcluster.h icaldirset.h icaldirsetimpl.h icalfileset.h icalfilesetimpl.h icalgauge.h icalgaugeimpl.h icalmessage.h icalset.h icalspanlist.h icalssyacc.h $(libicalssinclude_BDBHEADERS)

CONFIG_CLEAN_FILES = y.output

