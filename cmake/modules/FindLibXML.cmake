# - try to find libxml
#
# Once done this will define
#
#  LIBXML_FOUND - system has libxml
#  LIBXML_CFLAGS
#  LIBXML_LIBRARIES

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  if(PACKAGE_FIND_VERSION_COUNT GREATER 0)
    set(_libxml_version_cmp ">=${PACKAGE_FIND_VERSION}")
  endif()
  pkg_check_modules(_pc_libxml libxml-2.0${_libxml_version_cmp})
  if(_pc_libxml_FOUND)
    set(LIBXML_FOUND TRUE)
    set(LIBXML_CFLAGS "${_pc_libxml_CFLAGS}")
    set(LIBXML_LIBRARIES "${_pc_libxml_LIBRARIES}")
  endif()
endif()

mark_as_advanced(
  LIBXML_CFLAGS
  LIBXML_LIBRARIES
)
