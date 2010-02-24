# See xapian-bindings for an example of integrating autotools, swig and python

BUILT_SOURCES = _LibicalWrap.c

pyexec_LTLIBRARIES = _LibicalWrap.la

common_FILES =		\
__init__.py 		\
Libical.py 		\
test.py			\
Attendee.py 		\
Collection.py		\
Component.py		\
DerivedProperties.py	\
Duration.py		\
Error.py		\
Gauge.py		\
Period.py		\
Property.py		\
Store.py  		\
Time.py

# Install as _DATA rather than _SCRIPTS because we don't want to make these
# executable (they don't have a #! line).
pkgpython_DATA = 	\
$(common_FILES)		\
LibicalWrap.py

# To allow non-standard library names (ie those not prefixed by "lib") see:
# http://sources.redhat.com/automake/automake.html#Libtool-Modules
_LibicalWrap_la_SOURCES = _LibicalWrap.c
_LibicalWrap_la_LDFLAGS = -avoid-version -module -lc
_LibicalWrap_la_LIBADD =  $(top_srcdir)/src/libical/libical.la \
	$(top_srcdir)/src/libicalss/libicalss.la

AM_CPPFLAGS =				\
	-I$(top_builddir)		\
	-I$(top_srcdir)/src		\
	-I$(top_builddir)/src		\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/src/libical	\
	-I$(top_srcdir)/src/libicalss	\
	$(PY_CFLAGS)

_LibicalWrap.c:  LibicalWrap.i $(top_srcdir)/src/libical/ical.h $(top_srcdir)/src/libicalss/icalss.h
	swig -python -Wall $(AM_CPPFLAGS) -o _LibicalWrap.c LibicalWrap.i 

CLEANFILES = _LibicalWrap.c _LibicalWrap_wrap.doc  Libical.pyc _LibicalWrap.so

EXTRA_DIST = 		\
$(common_FILES)		\
LibicalWrap.i 		\
python-binding.txt 	\
ChangeLog


