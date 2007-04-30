cxx_inc=
cxx_libs=
regression_cxx = 
CXX=@CC@

if WITH_BDB4
bdb4_inc=-DWITH_BDB -I@BDB_DIR@/include
bdb4_libs=@BDB_DIR_LIB@/@BDB_LIB@
else
bdb4_inc=
bdb4_libs=
endif

noinst_PROGRAMS = copycluster regression parser stow recur testmime process

LDADD =  ../libicalss/.libs/libicalss.a $(cxx_libs) ../libical/.libs/libical.a $(bdb4_libs)

LIBS = @PTHREAD_LIBS@

INCLUDES = 				\
       -I. 				\
       -I$(srcdir) 			\
       -I../libical 			\
       -I$(srcdir)/../libical 		\
       -I../libicalss 			\
       -I$(srcdir)/../libicalss         \
	$(cxx_inc) $(bdb4_inc)

TESTS=regression
copycluster_SOURCES = copycluster.c
regression_SOURCES = 		\
	regression.c		\
	regression.h		\
	regression-component.c	\
	regression-classify.c	\
	regression-utils.c	\
	regression-recur.c	\
	regression-storage.c	\
	$(regression_cxx)

parser_SOURCES = icaltestparser.c
stow_SOURCES = stow.c
recur_SOURCES = recur.c 
testmime_SOURCES = testmime.c
process_SOURCES = process.c
