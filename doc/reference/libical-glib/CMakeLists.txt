# SPDX-FileCopyrightText: Milan Crha <mcrha@redhat.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

option(ENABLE_GTK_DOC "Use gtk-doc to build documentation" True)

if(NOT ENABLE_GTK_DOC)
  return()
endif()

if(STATIC_ONLY)
  message(FATAL_ERROR
    "You are attempting to build the Gtk docs, "
    "however that option is not supported when building static libraries only. "
    "Please disable the static only option (-DSTATIC_ONLY=False) "
    "if you really want to build the Gtk docs. Alternatively, you can "
    "disable building Gtk docs (by passing -DENABLE_GTK_DOC=False to cmake).")
endif()

# To regenerate libical-glib-docs.xml.in from current sources use these steps:
#   a) delete ${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml
#   b) go to ${CMAKE_CURRENT_BINARY_DIR} and run command:
#      gtkdoc-scan --module=libical-glib --source-dir=../../../src/libical-glib/
#                  --deprecated-guards="LIBICAL_GLIB_DISABLE_DEPRECATED"
#                  --ignore-headers=libical-glib-private.h --rebuild-sections --rebuild-types
#   c) generate the libical-glib-docs.xml file with command:
#      gtkdoc-mkdb --module=libical-glib --output-format=xml
#                  --source-dir=../../../src/libical-glib/ --xml-mode --name-space=i-cal
#   d) copy the newly created libical-glib-docs.xml
#      to ${CURRENT_SOURCE_DIR}/libical-glib-docs.xml.in
#   e) compare the changes in the file and return back what should be left,
#      like the replacement of the "[Insert title here]" and the <bookinfo/> content

if(ENABLE_GTK_DOC)
  include(GtkDoc)

  set(SOURCEDIRS
    ${PROJECT_BINARY_DIR}/src/libical-glib
  )

  set(DEPENDENCIES
    ical-glib
  )

  set(IGNORE_HEADERS
    libical-glib-private.h
  )

  add_gtkdoc(libical-glib libical-glib "LIBICAL_GLIB_DISABLE_DEPRECATED" SOURCEDIRS DEPENDENCIES IGNORE_HEADERS)

endif()
