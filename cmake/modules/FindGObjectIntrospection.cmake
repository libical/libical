# - try to find gobject-introspection 1.0
#
# Once done this will define
#
#  GObjectIntrospection_FOUND - system has gobject-introspection
#  GObjectIntrospection_SCANNER - the gobject-introspection scanner, g-ir-scanner
#  GObjectIntrospection_COMPILER - the gobject-introspection compiler, g-ir-compiler
#  GObjectIntrospection_GENERATE - the gobject-introspection generate, g-ir-generate
#  GObjectIntrospection_GIRDIR
#  GObjectIntrospection_TYPELIBDIR
#  GObjectIntrospection_CFLAGS
#  GObjectIntrospection_LIBS
#
# Copyright (C) 2010, Pino Toscano, <pino@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(_GIR_GET_PKGCONFIG_VAR _outvar _varname)
  execute_process(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=${_varname} gobject-introspection-1.0
    OUTPUT_VARIABLE _result
    RESULT_VARIABLE _null
  )

  if (_null)
  else()
    string(REGEX REPLACE "[\r\n]" " " _result "${_result}")
    string(REGEX REPLACE " +$" ""  _result "${_result}")
    separate_arguments(_result)
    set(${_outvar} ${_result} CACHE INTERNAL "")
  endif()
endmacro(_GIR_GET_PKGCONFIG_VAR)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  set(_gir_version_cmp)
  if(DEFINED GObjectIntrospection_FIND_VERSION)
    set(_gir_version_cmp "${GObjectIntrospection_FIND_VERSION}")
    set(_gir_version_cmp ">=${_gir_version_cmp}")
  endif()
  pkg_check_modules(_pc_gir gobject-introspection-1.0${_gir_version_cmp})
  if(_pc_gir_FOUND)
    set(GObjectIntrospection_FOUND TRUE)
    _gir_get_pkgconfig_var(GObjectIntrospection_SCANNER "g_ir_scanner")
    _gir_get_pkgconfig_var(GObjectIntrospection_COMPILER "g_ir_compiler")
    _gir_get_pkgconfig_var(GObjectIntrospection_GENERATE "g_ir_generate")
    _gir_get_pkgconfig_var(GObjectIntrospection_GIRDIR "girdir")
    _gir_get_pkgconfig_var(GObjectIntrospection_TYPELIBDIR "typelibdir")
    set(GObjectIntrospection_CFLAGS "${_pc_gir_CFLAGS}")
    set(GObjectIntrospection_LIBS "${_pc_gir_LIBS}")
  endif()
endif()

mark_as_advanced(
  GObjectIntrospection_SCANNER
  GObjectIntrospection_COMPILER
  GObjectIntrospection_GENERATE
  GObjectIntrospection_GIRDIR
  GObjectIntrospection_TYPELIBDIR
  GObjectIntrospection_CFLAGS
  GObjectIntrospection_LIBS
)
