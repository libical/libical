add_subdirectory(libical)
add_subdirectory(libicalss)
add_subdirectory(libicalvcal)
add_subdirectory(test)


########### install files ###############

install(FILES ical.h DESTINATION ${INCLUDE_INSTALL_DIR})


#original Makefile.am contents follow:

#SUBDIRS = libical libicalss libicalvcal test
#
#include_HEADERS = ical.h
