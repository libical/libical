include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_SOURCE_DIR}/src/libicalss )


########### next target ###############

SET(LibicalWrap_LIB_SRCS
   LibicalWrap.c
)

add_library(LibicalWrap ${LIBRARY_TYPE} ${LibicalWrap_LIB_SRCS})

target_link_libraries(LibicalWrap)

set_target_properties(LibicalWrap PROPERTIES VERSION ${LIBICAL_LIB_VERSION_STRING} SOVERSION ${LIBICAL_LIB_MAJOR_VERSION})
install(TARGETS LibicalWrap DESTINATION lib)


########### install files ###############




#original Makefile.am contents follow:

#
#lib_LTLIBRARIES = libLibicalWrap.la
#
#libLibicalWrap_la_SOURCES = LibicalWrap.c
#
#INCLUDES =				\
#	-I$(top_builddir)		\
#	-I$(top_srcdir)/src		\
#	-I$(top_builddir)/src		\
#	-I$(top_srcdir)/src/libical	\
#	-I$(top_builddir)/src/libical	\
#	-I$(top_srcdir)/src/libicalss	\
#	$(PY_CFLAGS) 	
#
#LDADD =  ../libical/libical.la ../libicalss/libicalss.la
#
#all: LibicalWrap.so
#
#LibicalWrap.c:  LibicalWrap.i
#	swig -python  -o LibicalWrap.c LibicalWrap.i 
#
## This part should be done with libtool, but I don't know how to do
## it. Libtool needs to generate a shared library in this directory
## regardless of the value of AM_DISABLE_SHARED
#LibicalWrap.so: LibicalWrap.c 
#	ld -shared -o LibicalWrap.so LibicalWrap.o ../libical/.libs/libical.a  ../libicalss/.libs/libicalss.a 
#
#CLEANFILES = LibicalWrap.c LibicalWrap_wrap.doc  Libical.pyc LibicalWrap.so
#
#EXTRA_DIST = 		\
#Libical.py 		\
#LibicalWrap.i 		\
#python-binding.txt 	\
#test.py			\
#Attendee.py 		\
#Collection.py		\
#Component.py		\
#DerivedProperties.py	\
#Duration.py		\
#Error.py		\
#Gauge.py		\
#Period.py		\
#Property.py		\
#Store.py  		\
#Time.py			\
#ChangeLog
#
#
