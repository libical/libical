include_directories(
  ${CMAKE_BINARY_DIR} 
  ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/libical ${CMAKE_BINARY_DIR}/src/libical 
  ${CMAKE_SOURCE_DIR}/src/libicalss ${CMAKE_BINARY_DIR}/src/libicalss
)

########### next target ###############

SET(doesnothing_SRCS
   access_components.c
   access_properties_and_parameters.c
   errors.c
   main.c
   parse_text.c
)

add_executable(doesnothing ${doesnothing_SRCS})

target_link_libraries(doesnothing ical icalss icalvcal ical icalss icalvcal)


########### install files ###############




#original Makefile.am contents follow:

#
#noinst_PROGRAMS = doesnothing
#
#if WITH_BDB4
#doesnothing_LDADD = ../src/libical/libical.la ../src/libicalss/libicalss.la ../src/libicalvcal/libicalvcal.la @BDB_DIR_LIB@/@BDB_LIB@
#else
#doesnothing_LDADD = ../src/libical/libical.la ../src/libicalss/libicalss.la ../src/libicalvcal/libicalvcal.la
#endif
#
#LIBS = @PTHREAD_LIBS@
#
#INCLUDES = -I. -I../src  -I../src/libical -I$(srcdir)/../src/libical -I../src/libicalss -I../src/libicalvcal 
#
#doesnothing_SOURCES = \
#	access_components.c \
#	access_properties_and_parameters.c \
#	errors.c \
#	main.c \
#	parse_text.c 
#
