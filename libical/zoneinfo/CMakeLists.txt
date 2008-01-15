install( FILES zones.tab  DESTINATION ${DATA_INSTALL_DIR}/libical/zoneinfo)

set(zoneinfodir Africa America America/Argentina America/Indiana America/Kentucky Antarctica Arctic Asia Atlantic Australia Europe Indian Pacific )
foreach (_zoneinfocurrentdir ${zoneinfodir})
	FILE(GLOB _icsfile "${_zoneinfocurrentdir}/*.ics" )
	install(FILES ${_icsfile} DESTINATION ${DATA_INSTALL_DIR}/libical/zoneinfo/${_zoneinfocurrentdir}/)
endforeach(_zoneinfocurrentdir ${zoneinfodir})

