# GIDocgen.cmake
#
# SPDX-FileCopyrightText: Corentin Noël <corentin.noel@collabora.com>
# SPDX-License-Identifier: BSD-3-Clause
#
# Macros to support developer documentation build from the introspection data
# with gi-docgen.
#
# generate_gi_documentation(_target _config _gir_path)
#    Generate the documentation from the given .gir file

include(CMakeParseArguments)

find_program(GI_DOCGEN gi-docgen)
if(NOT GI_DOCGEN)
  message(
    FATAL_ERROR
    "Cannot find gi-docgen. Install it or disable documentation generation with -DLIBICAL_GLIB_BUILD_DOCS=False"
  )
endif(NOT GI_DOCGEN)

# Generate the documentation from the GObject Introspection data
macro(generate_gi_documentation _target _config _gir_path)
  cmake_parse_arguments(GIDOCGEN_ARG "" "" "CONTENT_DIRS;INCLUDE_PATHS;TEMPLATE_DIRS" ${ARGN})

  set(EXTRA_ARGS)
  foreach(_item IN LISTS GIDOCGEN_ARG_CONTENT_DIRS)
    list(APPEND EXTRA_ARGS "--content-dir=${_item}")
  endforeach()
  foreach(_item IN LISTS GIDOCGEN_ARG_INCLUDE_PATHS)
    list(APPEND EXTRA_ARGS "--add-include-path=${_item}")
  endforeach()
  foreach(_item IN LISTS GIDOCGEN_ARG_TEMPLATE_DIRS)
    list(APPEND EXTRA_ARGS "--templates-dir=${_item}")
  endforeach()

  get_filename_component(_gir_filename ${_gir_path} NAME)
  set(_gir_target "${_gir_filename}")
  string(REPLACE "-" "_" _gir_target "${_gir_target}")
  string(REPLACE "." "_" _gir_target "${_gir_target}")
  set(_gir_target "gir-girs-${_gir_target}")
  add_custom_target(
    ${_target}-doc
    ALL
    COMMAND
      ${GI_DOCGEN} generate --config ${_config} --content-dir ${CMAKE_CURRENT_SOURCE_DIR} --no-namespace-dir
      --output-dir ${CMAKE_CURRENT_BINARY_DIR}/${_target} --quiet ${EXTRA_ARGS} ${_gir_path}
    DEPENDS
      ${_gir_target}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate documentation for ${_target}"
  )
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${_target}" DESTINATION "${SHARE_INSTALL_DIR}/doc")
endmacro(generate_gi_documentation)
