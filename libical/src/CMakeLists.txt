add_subdirectory(libical)
add_subdirectory(libicalss)
add_subdirectory(libicalvcal)
add_subdirectory(test)

if(MSVC)
install(FILES 
  ical.def
  icalss.def
  icalvcal.def
  DESTINATION
  ${LIB_INSTALL_DIR})
endif(MSVC)
