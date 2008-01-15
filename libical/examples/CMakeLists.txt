



########### next target ###############

set(doesnothing_SRCS 
   access_components.c 
   access_properties_and_parameters.c 
   errors.c 
   main.c 
   parse_text.c )


kde4_add_executable(doesnothing ${doesnothing_SRCS})

target_link_libraries(doesnothing  ${KDE4_KDECORE_LIBS} ical icalss icalvcal )


########### install files ###############





#original Makefile.am contents follow:

#
#noinst_PROGRAMS = doesnothing
#
#doesnothing_LDADD = ../src/libical/libical.la ../src/libicalss/libicalss.la ../src/libicalvcal/libicalvcal.la
#
##LIBS = @PTHREAD_LIBS@
#
#INCLUDES = -I. -I../src/libical -I../src/libicalss -I../src/libicalvcal 
#
#doesnothing_SOURCES = \
#	access_components.c \
#	access_properties_and_parameters.c \
#	errors.c \
#	main.c \
#	parse_text.c 
#
