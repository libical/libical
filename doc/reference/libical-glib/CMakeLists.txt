include(GtkDoc)

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

  set(SOURCEDIRS
    ${CMAKE_BINARY_DIR}/src/libical-glib
  )

  set(DEPENDENCIES
    ical-glib
  )

  set(IGNORE_HEADERS
    libical-glib-private.h
  )

  add_gtkdoc(libical-glib libical-glib "LIBICAL_GLIB_DISABLE_DEPRECATED" SOURCEDIRS DEPENDENCIES IGNORE_HEADERS)

endif()
