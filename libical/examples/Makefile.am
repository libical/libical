
noinst_PROGRAMS = doesnothing

if WITH_BDB4
doesnothing_LDADD = ../src/libical/libical.la ../src/libicalss/libicalss.la ../src/libicalvcal/libicalvcal.la @BDB_DIR_LIB@/@BDB_LIB@
else
doesnothing_LDADD = ../src/libical/libical.la ../src/libicalss/libicalss.la ../src/libicalvcal/libicalvcal.la
endif

LIBS = @PTHREAD_LIBS@

INCLUDES = -I. -I../src/libical -I$(srcdir)/../src/libical -I../src/libicalss -I../src/libicalvcal 

doesnothing_SOURCES = \
	access_components.c \
	access_properties_and_parameters.c \
	errors.c \
	main.c \
	parse_text.c 

