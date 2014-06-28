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

# Install as python source so the code gets byte-compiled at install time.
pkgpython_PYTHON = 	\
$(common_FILES)		\
LibicalWrap.py

# To allow non-standard library names (ie those not prefixed by "lib") see:
# http://sources.redhat.com/automake/automake.html#Libtool-Modules
_LibicalWrap_la_SOURCES = _LibicalWrap.c
_LibicalWrap_la_LDFLAGS = -avoid-version -module -lc
_LibicalWrap_la_LIBADD =  $(top_builddir)/src/libical/libical.la \
	$(top_builddir)/src/libicalss/libicalss.la

AM_CPPFLAGS =				\
	-I$(top_builddir)		\
	-I$(top_srcdir)/src		\
	-I$(top_builddir)/src		\
	-I$(top_srcdir)/src/libical	\
	-I$(top_builddir)/src/libical	\
	-I$(top_srcdir)/src/libicalss	\
	$(PY_CFLAGS)

_LibicalWrap.c:  $(srcdir)/LibicalWrap.i $(srcdir)/*.i $(top_builddir)/src/libical/ical.h $(top_builddir)/src/libicalss/icalss.h
	swig -python -Wall $(AM_CPPFLAGS) -o _LibicalWrap.c $(srcdir)/LibicalWrap.i 

CLEANFILES = _LibicalWrap.c _LibicalWrap_wrap.doc  Libical.pyc LibicalWrap.py _LibicalWrap.so 

EXTRA_DIST = 		\
$(common_FILES)		\
LibicalWrap.i 		\
python-binding.txt 	\
ChangeLog


