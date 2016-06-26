# - try to find glib
#
# Once done this will define
#
#  GLIB_FOUND - system has GLib 2.0
#  GLIB_CFLAGS
#  GLIB_LIBRARIES

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  if(PACKAGE_FIND_VERSION_COUNT GREATER 0)
    set(_glib_version_cmp ">=${PACKAGE_FIND_VERSION}")
  endif()
  pkg_check_modules(_pc_glib glib-2.0${_glib_version_cmp} gobject-2.0${_glib_version_cmp})
  if(_pc_glib_FOUND)
    set(GLIB_FOUND TRUE)
    set(GLIB_CFLAGS "${_pc_glib_CFLAGS}")
    set(GLIB_LIBRARIES "${_pc_glib_LDFLAGS}")
  endif()
endif()

mark_as_advanced(
  GLIB_CFLAGS
  GLIB_LIBRARIES
)
