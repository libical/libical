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
# SPDX-FileCopyrightText: 2010, Pino Toscano, <pino@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
#

# Get gobject-introspection's specified pkg-config variable
macro(_GIR_GET_PKGCONFIG_VAR _outvar _varname)
  execute_process(
    COMMAND
      ${PKG_CONFIG_EXECUTABLE} --variable=${_varname} gobject-introspection-1.0
    OUTPUT_VARIABLE _result
    RESULT_VARIABLE _null
  )

  if(_null)
  else()
    string(REGEX REPLACE "[\r\n]" " " _result "${_result}")
    string(REGEX REPLACE " +$" "" _result "${_result}")
    separate_arguments(_result UNIX_COMMAND ${_result})
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
  pkg_check_modules(
    _pc_gir
    gobject-introspection-1.0${_gir_version_cmp}
  )
  if(_pc_gir_FOUND)
    set(GObjectIntrospection_FOUND TRUE)
    _GIR_GET_PKGCONFIG_VAR(GObjectIntrospection_SCANNER "g_ir_scanner")
    _GIR_GET_PKGCONFIG_VAR(GObjectIntrospection_COMPILER "g_ir_compiler")
    _GIR_GET_PKGCONFIG_VAR(GObjectIntrospection_GENERATE "g_ir_generate")
    _GIR_GET_PKGCONFIG_VAR(GObjectIntrospection_GIRDIR "girdir")
    _GIR_GET_PKGCONFIG_VAR(GObjectIntrospection_TYPELIBDIR "typelibdir")
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
