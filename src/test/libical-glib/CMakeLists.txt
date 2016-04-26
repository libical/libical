find_program(PYTHON3 python3)

list(APPEND TEST_FILES
	array.py
	attach.py
	component.py
	comprehensive.py
	duration-type.py
	error.py
	parameter.py
	period-type.py
	property.py
	recurrence-type.py
	timezone.py
	value.py
)

if(PYTHON3)

	list(APPEND test_env
		"GI_TYPELIB_PATH=${CMAKE_BINARY_DIR}/src/libical-glib;$ENV{GI_TYPELIB_PATH}"
		"LD_LIBRARY_PATH=${LIBRARY_OUTPUT_PATH};$ENV{LD_LIBRARY_PATH}"
	)

	foreach(test_file IN LISTS TEST_FILES)

		string(REPLACE ".py" "" test_name ${test_file})
		set(test_name "libical-glib-${test_name}")

		add_test(
			NAME ${test_name}
			COMMAND ${PYTHON3} ${CMAKE_CURRENT_SOURCE_DIR}/${test_file}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)

		set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "${test_env}")

	endforeach(test_file)

else(PYTHON3)
	message(WARNING "python3 not found, cannot run libical-glib tests")
endif(PYTHON3)
