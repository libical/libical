
lib_LTLIBRARIES = libLibicalWrap.la

libLibicalWrap_la_SOURCES = LibicalWrap.c

EXTRA_DIST = \
Libical.py \
LibicalWrap.i \
python-binding.txt \
test.py

CLEANFILES = LibicalWrap.c LibicalWrap_wrap.doc  Libical.pyc LibicalWrap.so

PYTHONINC = /usr/local/include/python2.0
PYTHONLIB = /usr/local/lib/python2.0/config

INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	-I$(top_srcdir)/src/libical	\
	-I$(srcdir)/libical     \
	-I$(PYTHONINC) \
	-I$(PYTHONLIB) 

LDADD =  ../libical/libical.la 

all: LibicalWrap.so

LibicalWrap.c:  LibicalWrap.i
	swig -python  -o LibicalWrap.c LibicalWrap.i 

# This part should be done with libtool, but I don't know how to do
# it. Libtool needs to generate a shared library in this directory
# regardless of the value of AM_DISABLE_SHARED
LibicalWrap.so:
	ld -shared -o LibicalWrap.so LibicalWrap.o ../libical/.libs/libical.a 
