if WITH_CXX
cxx_progs=testcxx
cxx_libs=../libical/.libs/libical_cxx.a
testcxx_SOURCES = testcxx.cpp
else
cxx_progs=
cxx_libs=
endif


noinst_PROGRAMS = copycluster regression parser storage stow recur testmime testvcal process $(cxx_progs)

LDADD =  ../libicalss/.libs/libicalss.a ../libicalvcal/.libs/libicalvcal.a $(cxx_libs) ../libical/.libs/libical.a 

LIBS = -lpthread 

storage_LDADD = ../libicalss/.libs/libicalss.a ../libicalvcal/.libs/libicalvcal.a $(cxx_libs) ../libical/.libs/libical.a @BDB_DIR@/lib/libdb-4.0.la

INCLUDES = 				\
       -I. 				\
       -I../libicalvcal 		\
       -I$(srcdir)/../libicalvcal 	\
       -I$(srcdir) 			\
       -I../libical 			\
       -I$(srcdir)/../libical 		\
       -I../libicalss 			\
       -I$(srcdir)/../libicalss         \
       -I@BDB_DIR@/include 

copycluster_SOURCES = copycluster.c
regression_SOURCES = regression.c regression-component.c regression-classify.c regression-utils.c\
		     regression-recur.c
parser_SOURCES = icaltestparser.c
storage_SOURCES = storage.c
stow_SOURCES = stow.c
testvcal_SOURCES = testvcal.c
recur_SOURCES = recur.c 
testmime_SOURCES = testmime.c
process_SOURCES = process.c
