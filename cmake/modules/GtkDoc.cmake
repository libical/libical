# GtkDoc.cmake
#
# SPDX-FileCopyrightText: Milan Crha <mcrha@redhat.com>
# SPDX-License-Identifier: BSD-3-Clause
#
# Macros to support developer documentation build from sources with gtk-doc.
#
# Note that every target and dependency should be defined before the macro is
# called, because it uses information from those targets.
#
# add_gtkdoc(_module _namespace _deprecated_guards _srcdirsvar _depsvar _ignoreheadersvar)
#    Adds rules to build developer documentation using gtk-doc for some part.
#    Arguments:
#       _module - the module name, like 'camel'; it expects ${_part}-docs.sgml.in in the CMAKE_CURRENT_SOURCE_DIR
#       _namespace - namespace for symbols
#       _deprecated_guards - define name, which guards deprecated symbols
#       _srcdirsvar - variable with dirs where the source files are located
#       _depsvar - a variable with dependencies (targets)
#       _ignoreheadersvar - a variable with a set of header files to ignore
#
# It also adds custom target gtkdoc-rebuild-${_module}-sgml to rebuild the sgml.in
# file based on the current sources.

find_program(GTKDOC_SCAN gtkdoc-scan)
find_program(GTKDOC_SCANGOBJ gtkdoc-scangobj)
find_program(GTKDOC_MKDB gtkdoc-mkdb)
find_program(GTKDOC_MKHTML gtkdoc-mkhtml)
find_program(GTKDOC_FIXXREF gtkdoc-fixxref)

if(NOT
   (GTKDOC_SCAN
    AND GTKDOC_MKDB
    AND GTKDOC_MKHTML
    AND GTKDOC_FIXXREF)
)
  message(FATAL_ERROR "Cannot find all gtk-doc binaries, install them or use -DENABLE_GTK_DOC=OFF  instead")
  return()
endif()

if(NOT TARGET gtkdocs)
  add_custom_target(gtkdocs ALL COMMENT "Target to run gtkdoc for all modules")
endif()

if(NOT TARGET gtkdoc-rebuild-sgmls)
  add_custom_target(gtkdoc-rebuild-sgmls COMMENT "Target to rebuild sgml for all modules")
endif()

# cmake-lint: disable=R0912,R0915
macro(
  add_gtkdoc
  _module
  _namespace
  _deprecated_guards
  _srcdirsvar
  _depsvar
  _ignoreheadersvar
  _extra_ldflags
)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${_module}-docs.sgml.in ${CMAKE_CURRENT_BINARY_DIR}/${_module}-docs.sgml @ONLY
  )

  set(OUTPUT_DOCDIR ${SHARE_INSTALL_DIR}/gtk-doc/html/${_module})

  set(_filedeps)
  set(_srcdirs)
  foreach(_srcdir ${${_srcdirsvar}})
    set(_srcdirs ${_srcdirs} --source-dir="${_srcdir}")
    file(GLOB _files ${_srcdir}/*.h* ${_srcdir}/*.c*)
    list(APPEND _filedeps ${_files})
  endforeach()

  if(APPLE)
    if(NOT DEFINED ENV{XML_CATALOG_FILES})
      message(FATAL_ERROR "On OSX, please run \'export XML_CATALOG_FILES=/usr/local/etc/xml/catalog\' first; "
                          "else the gtk entities cannot be located."
      )
    endif()
  endif()

  set(_scangobj_deps)
  set(_scangobj_cflags_list)
  set(_scangobj_cflags "-Wno-error=unused-parameter")
  set(_scangobj_ldflags "${_extra_ldflags}")
  set(_scangobj_ld_lib_dirs "")

  # LIB_INSTALL_DIR might be a relative path, (eg. "lib")
  cmake_path(SET _nativeLibPath "${LIB_INSTALL_DIR}")
  cmake_path(IS_RELATIVE _nativeLibPath _libIsRelative)
  if(_libIsRelative)
    #join to install prefix
    set(_libraryDir "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
  else()
    set(_libraryDir "${LIB_INSTALL_DIR}")
  endif()

  foreach(opt IN LISTS ${_depsvar})
    if(TARGET ${opt})
      set(_target_type)
      get_target_property(_target_type ${opt} TYPE)
      if((_target_type STREQUAL "STATIC_LIBRARY")
         OR (_target_type STREQUAL "SHARED_LIBRARY")
         OR (_target_type STREQUAL "MODULE_LIBRARY")
      )
        set(_compile_options)
        set(_link_libraries)

        get_target_property(_compile_options ${opt} COMPILE_OPTIONS)
        get_target_property(_link_libraries ${opt} LINK_LIBRARIES)

        list(APPEND _scangobj_cflags_list ${_compile_options})
        list(APPEND _scangobj_deps ${_link_libraries})

        unset(_compile_options)
        unset(_link_libraries)
      endif()
      unset(_target_type)
    endif()

    list(APPEND _scangobj_deps ${opt})
  endforeach()

  # Add it as the last, thus in-tree headers have precedence
  list(APPEND _scangobj_cflags_list -I${INCLUDE_INSTALL_DIR})
  list(APPEND _scangobj_cflags_list ${CMAKE_C_FLAGS})

  if(_scangobj_deps)
    list(REMOVE_DUPLICATES _scangobj_deps)
  endif()
  if(_scangobj_cflags_list)
    list(REMOVE_DUPLICATES _scangobj_cflags_list)
  endif()

  foreach(opt IN LISTS _scangobj_cflags_list)
    set(_scangobj_cflags "${opt} ${_scangobj_cflags}")
  endforeach()

  foreach(opt IN LISTS _scangobj_deps)
    if(TARGET ${opt})
      set(_target_type)
      get_target_property(_target_type ${opt} TYPE)
      if((_target_type STREQUAL "STATIC_LIBRARY")
         OR (_target_type STREQUAL "SHARED_LIBRARY")
         OR (_target_type STREQUAL "MODULE_LIBRARY")
      )
        set(_output_name "")
        get_target_property(_output_name ${opt} OUTPUT_NAME)
        if(NOT _output_name)
          set(_output_name ${opt})
        endif()
        set(_scangobj_ldflags "${_scangobj_ldflags} -L$<TARGET_FILE_DIR:${opt}> -l${_output_name}")

        if(_target_type STREQUAL "SHARED_LIBRARY" OR (_target_type STREQUAL "MODULE_LIBRARY"))
          set(_scangobj_ld_lib_dirs "${_scangobj_ld_lib_dirs}:$<TARGET_FILE_DIR:${opt}>")
        endif()
        unset(_output_name)
      endif()
      unset(_target_type)
    endif()
  endforeach()

  # Add extra flags from LDFLAGS environment variable
  set(_scangobj_ldflags "${_scangobj_ldflags} ${CMAKE_SHARED_LINKER_FLAGS}")

  foreach(opt IN LISTS _scangobj_deps)
    if(NOT TARGET ${opt})
      set(_scangobj_ldflags "${_scangobj_ldflags} ${opt}")
    endif()
  endforeach()

  # Add it as the last, thus in-tree libs have precedence
  set(_scangobj_ldflags "${_scangobj_ldflags} -L${_libraryDir}")

  if(APPLE)
    set(ld_lib_path "DYLD_LIBRARY_PATH=${_scangobj_ld_lib_dirs}:${_libraryDir}")
    if(DEFINED DYLD_LIBRARY_PATH)
      set(ld_lib_path "${ld_lib_path}:$ENV{DYLD_LIBRARY_PATH}")
    endif()
  elseif(NOT WIN32 AND NOT WINCE) #ie. unix-like
    set(ld_lib_path "LD_LIBRARY_PATH=${_scangobj_ld_lib_dirs}:${_libraryDir}")
    if(DEFINED LD_LIBRARY_PATH)
      set(ld_lib_path "${ld_lib_path}:$ENV{LD_LIBRARY_PATH}")
    endif()
  endif()
  set(_scangobj_prefix ${CMAKE_COMMAND} -E env "${ld_lib_path}")

  #  if(NOT (_scangobj_cflags STREQUAL ""))
  #    set(_scangobj_cflags --cflags "${_scangobj_cflags}")
  #  endif()

  #  if(NOT (_scangobj_ldflags STREQUAL ""))
  #    set(_scangobj_ldflags "--ldflags ${_scangobj_ldflags}")
  #  endif()

  add_custom_command(
    OUTPUT html/index.html
    COMMAND ${GTKDOC_SCAN} --module=${_module} --deprecated-guards="${_deprecated_guards}"
            --ignore-headers="${${_ignoreheadersvar}}" --rebuild-sections --rebuild-types ${_srcdirs}
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}" ${_scangobj_prefix} ${GTKDOC_SCANGOBJ}
            --module=${_module} --cflags=${_scangobj_cflags} --ldflags=${_scangobj_ldflags}
    COMMAND
      ${GTKDOC_MKDB} --module=${_module} --name-space=${_namespace}
      --main-sgml-file="${CMAKE_CURRENT_BINARY_DIR}/${_module}-docs.sgml" --sgml-mode --output-format=xml ${_srcdirs}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/html" ${GTKDOC_MKHTML} --path=.. ${_module}
            ../${_module}-docs.sgml
    COMMAND ${GTKDOC_FIXXREF} --module=${_module} --module-dir=html --extra-dir=.. --html-dir="${OUTPUT_DOCDIR}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${_module}-docs.sgml" ${_filedeps}
    COMMENT "Generating ${_module} documentation"
  )

  add_custom_target(
    gtkdoc-${_module}
    DEPENDS html/index.html
    COMMENT "Target for running gtkdoc for module"
  )

  if(${_depsvar})
    add_dependencies(gtkdoc-${_module} ${${_depsvar}})
  endif(${_depsvar})

  add_dependencies(gtkdocs gtkdoc-${_module})

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${OUTPUT_DOCDIR})

  # ***************************************
  # sgml.in file rebuild, unconditional
  # ***************************************
  add_custom_target(
    gtkdoc-rebuild-${_module}-sgml
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/tmp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/tmp"
    COMMAND
      ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/tmp" ${GTKDOC_SCAN} --module=${_module}
      --deprecated-guards="${_deprecated_guards}" --ignore-headers="${${_ignoreheadersvar}}" --rebuild-sections
      --rebuild-types ${_srcdirs}
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}" ${_scangobj_prefix} ${GTKDOC_SCANGOBJ}
            --module=${_module} ${_scangobj_cflags} ${_scangobj_ldflags}
    COMMAND
      ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/tmp" ${GTKDOC_MKDB} --module=${_module}
      --name-space=${_namespace} --main-sgml-file="${CMAKE_CURRENT_BINARY_DIR}/tmp/${_module}-docs.sgml" --sgml-mode
      --output-format=xml ${_srcdirs}
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/tmp/${_module}-docs.sgml
            ${CMAKE_CURRENT_SOURCE_DIR}/${_module}-docs.sgml.in
    COMMAND ${CMAKE_COMMAND} -E echo "File '${CMAKE_CURRENT_SOURCE_DIR}/${_module}-docs.sgml.in' overwritten, "
            "make sure to replace generated strings with proper content before committing."
    COMMENT "Target to rebuild the sgml for the specified module"
  )

  add_dependencies(gtkdoc-rebuild-sgmls gtkdoc-rebuild-${_module}-sgml)

  unset(_scangobj_prefix)
  unset(_scangobj_deps)
  unset(_scangobj_cflags_list)
  unset(_scangobj_cflags)
  unset(_scangobj_ldflags)
  unset(_scangobj_ld_lib_dirs)
endmacro()
