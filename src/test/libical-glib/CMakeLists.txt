find_program(PYTHON3 python3)
set_package_properties(PYTHON3 PROPERTIES TYPE REQUIRED PURPOSE "Required by the libical build system.")
add_feature_info("libical-glib testing" PYTHON3 "python3 is required to run the regression tests")

list(APPEND TEST_FILES
  array.py
  attach.py
  component.py
  comprehensive.py
  duration.py
  error.py
  misc.py
  parameter.py
  period.py
  property.py
  recurrence.py
  timezone.py
  value.py
)

if(PYTHON3)
  list(APPEND test_env
    "GI_TYPELIB_PATH=${CMAKE_BINARY_DIR}/src/libical-glib;$ENV{GI_TYPELIB_PATH}"
    "LD_LIBRARY_PATH=${LIBRARY_OUTPUT_PATH};$ENV{LD_LIBRARY_PATH}"
    "ZONEINFO_DIRECTORY=${CMAKE_SOURCE_DIR}/zoneinfo"
  )

  foreach(test_file IN LISTS TEST_FILES)
    string(REPLACE ".py" "" test_name ${test_file})
    set(test_name "libical-glib-${test_name}")

    add_test(
      NAME ${test_name}
      COMMAND ${PYTHON3} -I ${CMAKE_CURRENT_SOURCE_DIR}/${test_file}
    )
    setprops(${test_name})
    set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "${test_env}")

  endforeach()
endif()
