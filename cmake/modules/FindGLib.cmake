#
# SPDX-FileCopyrightText: Milan Crha <mcrha@redhat.com>
# SPDX-License-Identifier: BSD-3-Clause
#
# - try to find glib
# Once done this will define
#
#  GLIB_FOUND - system has GLib 2.0
#  GLIB_CFLAGS
#  GLIB_LIBRARIES

set_package_properties(
  GLib
  PROPERTIES
    DESCRIPTION
      "A library of handy utility functions"
    URL
      "https://www.gtk.org"
)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  set(_glib_version_cmp)
  if(DEFINED GLib_FIND_VERSION)
    set(_glib_version_cmp ${GLib_FIND_VERSION})
    set(_glib_version_cmp ">=${_glib_version_cmp}")
  endif()
  pkg_check_modules(
    _pc_glib
    glib-2.0${_glib_version_cmp}
    gobject-2.0${_glib_version_cmp}
  )
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
