# SPDX-FileCopyrightText: 2010, Pino Toscano, <pino at kde.org>
# SPDX-License-Identifier: BSD-3-Clause

# Generate list from another list, but with each item prepended with a prefix
macro(_gir_list_prefix _outvar _listvar _prefix)
  set(${_outvar})
  foreach(_item IN LISTS ${_listvar})
    list(APPEND ${_outvar} ${_prefix}${_item})
  endforeach()
endmacro(_gir_list_prefix)

# cmake-lint: disable=R0915
macro(gir_add_introspections introspections_girs)
  set(_gir_girs)
  set(_gir_typelibs)

  foreach(gir IN LISTS ${introspections_girs})
    set(_gir_name "${gir}")

    ## Transform the gir filename to something which can reference through a variable
    ## without automake/make complaining, eg Gtk-2.0.gir -> Gtk_2_0_gir
    string(
      REPLACE "-"
      "_"
      _gir_name
      "${_gir_name}"
    )
    string(
      REPLACE "."
      "_"
      _gir_name
      "${_gir_name}"
    )

    # Namespace and Version is either fetched from the gir filename
    # or the _NAMESPACE/_VERSION variable combo
    set(_gir_namespace "")
    if(DEFINED ${_gir_name}_NAMESPACE)
      set(_gir_namespace "${${_gir_name}_NAMESPACE}")
    endif()
    if(_gir_namespace STREQUAL "")
      string(
        REGEX REPLACE "([^-]+)-.*"
        "\\1"
        _gir_namespace
        "${gir}"
      )
    endif()

    set(_gir_version "")
    if(DEFINED ${_gir_name}_VERSION)
      set(_gir_version "${${_gir_name}_VERSION}")
    endif()
    if(_gir_version STREQUAL "")
      string(
        REGEX REPLACE ".*-([^-]+).gir"
        "\\1"
        _gir_version
        "${gir}"
      )
    endif()

    # _PROGRAM is an optional variable which needs its own --program argument
    set(_gir_program "")
    if(DEFINED ${_gir_name}_PROGRAM)
      set(_gir_program "${${_gir_name}_PROGRAM}")
    endif()
    if(NOT _gir_program STREQUAL "")
      set(_gir_program "--program=${_gir_program}")
    endif()

    # _SCANNERFLAGS is optional
    set(_gir_scannerflags "")
    if(DEFINED ${_gir_name}_SCANNERFLAGS)
      set(_gir_scannerflags "${${_gir_name}_SCANNERFLAGS}")
    endif()

    # _FILES
    set(_gir_files "")
    if(DEFINED ${_gir_name}_FILES)
      set(_gir_files "${${_gir_name}_FILES}")
    else()
      message(
        ERROR
        "Unspecified or empty ${_gir_name}_FILES variable"
      )
    endif()

    # Variables which provides a list of things
    _gir_list_prefix(_gir_libraries ${_gir_name}_LIBS "--library=")
    _gir_list_prefix(_gir_packages ${_gir_name}_PACKAGES "--pkg=")
    _gir_list_prefix(_gir_includes ${_gir_name}_INCLUDES "--include=")

    # Reuse the LIBTOOL variable from by automake if it's set
    set(_gir_libtool "--no-libtool")

    add_custom_command(
      OUTPUT
        ${gir}
      COMMAND
        ${GObjectIntrospection_SCANNER} ${GObjectIntrospection_SCANNER_ARGS} --namespace=${_gir_namespace}
        --nsversion=${_gir_version} ${_gir_libtool} ${_gir_program} ${_gir_libraries} ${_gir_packages} ${_gir_includes}
        ${_gir_scannerflags} ${${_gir_name}_CFLAGS} ${_gir_files} --output ${CMAKE_CURRENT_BINARY_DIR}/${gir}
        --accept-unprefixed
      DEPENDS
        ${_gir_files}
        ${${_gir_name}_LIBS}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      VERBATIM
      COMMENT "Run the gobject introspection scanner"
    )
    list(APPEND _gir_girs ${CMAKE_CURRENT_BINARY_DIR}/${gir})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${gir} DESTINATION ${SHARE_INSTALL_DIR}/gir-1.0)

    string(
      REPLACE ".gir"
      ".typelib"
      _typelib
      "${gir}"
    )
    add_custom_command(
      OUTPUT
        ${_typelib}
      COMMAND
        ${GObjectIntrospection_COMPILER} --includedir=. ${CMAKE_CURRENT_BINARY_DIR}/${gir} -o
        ${CMAKE_CURRENT_BINARY_DIR}/${_typelib}
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${gir}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Run the gobject introspection compiler"
    )
    list(APPEND _gir_typelibs ${CMAKE_CURRENT_BINARY_DIR}/${_typelib})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_typelib} DESTINATION ${LIB_INSTALL_DIR}/girepository-1.0)
  endforeach()

  add_custom_target(
    gir-girs-${_gir_name}
    ALL
    DEPENDS
      ${_gir_girs}
    COMMENT "Target for the gobject introspection compiler"
  )
  add_custom_target(
    gir-typelibs-${_gir_name}
    ALL
    DEPENDS
      ${_gir_typelibs}
    COMMENT "Target for the gobject introspection typelibs"
  )
endmacro(gir_add_introspections)
