find_program(GTKDOC_SCAN gtkdoc-scan)
find_program(GTKDOC_MKDB gtkdoc-mkdb)
find_program(GTKDOC_MKHTML gtkdoc-mkhtml)
find_program(GTKDOC_FIXXREF gtkdoc-fixxref)

if(NOT (GTKDOC_SCAN AND GTKDOC_MKDB AND GTKDOC_MKHTML AND GTKDOC_FIXXREF))
	message(WARNING "Cannot find all gtk-doc binaries, skipping API reference generation for libical-glib")
	return()
endif()

# To regenerate libical-glib-docs.xml.in from current sources use these steps:
#   a) delete ${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml
#   b) go to ${CMAKE_CURRENT_BINARY_DIR} and run command:
#      gtkdoc-scan --module=libical-glib --source-dir=../../../src/libical-glib/ --deprecated-guards="LIBICAL_GLIB_DISABLE_DEPRECATED" --ignore-headers=libical-glib-private.h --rebuild-sections --rebuild-types
#   c) generate the libical-glib-docs.xml file with command:
#      gtkdoc-mkdb --module=libical-glib --output-format=xml --source-dir=../../../src/libical-glib/ --xml-mode --name-space=i-cal
#   d) copy the newly created libical-glib-docs.xml
#      to ${CURRENT_SOURCE_DIR}/libical-glib-docs.xml.in
#   e) compare the changes in the file and return back what should be left,
#      like the replacement of the "[Insert title here]" and the <bookinfo/> content

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/libical-glib-docs.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml
	@ONLY
)

set(OUTPUT_DOCDIR ${SHARE_INSTALL_DIR}/gtk-doc/html/libical-glib)

add_custom_command(OUTPUT xml/libical-glib-doc.bottom
	COMMAND ${GTKDOC_SCAN} --module=libical-glib
		--source-dir="${CMAKE_BINARY_DIR}/src/libical-glib"
		--deprecated-guards="LIBICAL_GLIB_DISABLE_DEPRECATED"
		--ignore-headers=libical-glib-private.h
		--rebuild-sections
		--rebuild-types

	COMMAND ${GTKDOC_MKDB}
		--module=libical-glib
		--name-space=i-cal
		--main-sgml-file="${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml"
		--xml-mode
		--output-format=xml
		--source-dir="${CMAKE_BINARY_DIR}/src/libical-glib"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/html"
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml"
)

add_custom_command(OUTPUT html/index.html
	COMMAND ${GTKDOC_MKHTML}
		--path=..
		libical-glib
		../libical-glib-docs.xml
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libical-glib-docs.xml" "${CMAKE_CURRENT_BINARY_DIR}/xml/libical-glib-doc.bottom"
	COMMENT "Generating libical-glib documentation"
)

add_custom_target(documentation ALL
	COMMAND ${GTKDOC_FIXXREF}
		--module=libical-glib
		--module-dir=.
		--extra-dir=..
		--html-dir=${OUTPUT_DOCDIR}
	DEPENDS html/index.html
	COMMENT "Generating libical-glib documentation"
)
add_dependencies(documentation ical-glib)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
	DESTINATION ${OUTPUT_DOCDIR}
	PATTERN "html/*")
