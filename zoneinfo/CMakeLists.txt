
########### install files ###############

install(FILES  zones.tab DESTINATION ${SHARE_INSTALL_DIR}/libical/zoneinfo)

set(zoneinfodir
  Africa 
  America
  America/North_Dakota
  America/Indiana
  America/Kentucky
  America/Argentina 
  Antarctica
  Arctic
  Asia
  Atlantic
  Australia
  Europe
  Indian
  Pacific
  )
foreach (_zoneinfocurrentdir ${zoneinfodir})
  FILE(GLOB _icsfile "${_zoneinfocurrentdir}/*.ics" )
  install(FILES ${_icsfile} DESTINATION ${SHARE_INSTALL_DIR}/libical/zoneinfo/${_zoneinfocurrentdir}/)
endforeach(_zoneinfocurrentdir ${zoneinfodir})

#original Makefile.am contents follow:

#
#zoneinfodatadir = $(datadir)/$(PACKAGE)/zoneinfo
#
#zoneinfodata_DATA = zones.tab
#
#DIRS = \
#	Africa			\
#	America			\
#	America/North_Dakota	\
#	America/Indiana		\
#	America/Kentucky	\
#	America/Argentina	\
#	Antarctica		\
#	Arctic			\
#	Asia			\
#	Atlantic		\
#	Australia		\
#	Europe			\
#	Indian			\
#	Pacific
#
#
#install-data-local:
#	@$(NORMAL_INSTALL)
#	for dir in $(DIRS); do \
#	  $(mkinstalldirs) $(DESTDIR)$(datadir)/$(PACKAGE)/zoneinfo/$$dir; \
#	  for tzfile in $(srcdir)/$$dir/*.ics; do \
#	    $(INSTALL_DATA) $$tzfile $(DESTDIR)$(datadir)/$(PACKAGE)/zoneinfo/$$dir; \
#	  done \
#	done
#
#dist-hook:
#	for dir in $(DIRS); do \
#	  mkdir $(distdir)/$$dir; \
#	  for tzfile in $(srcdir)/$$dir/*.ics; do \
#	    cp -p $$tzfile $(distdir)/$$dir; \
#	  done \
#	done
#
#EXTRA_DIST = $(zoneinfodata_DATA)
