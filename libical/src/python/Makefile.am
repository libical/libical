
EXTRA_DIST = \
Libical.py \
LibicalWrap.i \
python-binding.txt \
test.py

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


all: LibicalWrap.so 

# Yeah, I should be using libtool and automake here, but I am too lazy
# to try to figure it out.

LibicalWrap_wrap.c:  LibicalWrap.i
	swig -python LibicalWrap.i   

LibicalWrap_wrap.o: LibicalWrap_wrap.c
	$(CC) -c LibicalWrap_wrap.c -DHAVE_CONFIG_H $(INCLUDES)

LibicalWrap.so: LibicalWrap_wrap.o
	ld -shared LibicalWrap_wrap.o -L../libical/.libs -lical -o LibicalWrap.so
