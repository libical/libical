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
  set(GI_TYPELIB_PATH_STR "${PROJECT_BINARY_DIR}/src/libical-glib")
  if(DEFINED ENV{GI_TYPELIB_PATH})
    if(NOT ENV{GI_TYPELIB_PATH} STREQUAL "")
      set(GI_TYPELIB_PATH_STR "${GI_TYPELIB_PATH_STR}:$ENV{GI_TYPELIB_PATH}")
    endif()
  endif()
  set(LIBRARY_PATH_STR "${LIBRARY_OUTPUT_PATH}")
  if(DEFINED ENV{LD_LIBRARY_PATH})
    if(NOT ENV{LD_LIBRARY_PATH} STREQUAL "")
      set(LIBRARY_PATH_STR "${LIBRARY_PATH_STR}:$ENV{LD_LIBRARY_PATH}")
    endif()
  endif()
  if(DEFINED ENV{DYLD_LIBRARY_PATH})
    if(NOT ENV{DYLD_LIBRARY_PATH} STREQUAL "")
      set(LIBRARY_PATH_STR "${LIBRARY_PATH_STR}:$ENV{DYLD_LIBRARY_PATH}")
    endif()
  endif()

  list(APPEND test_env "GI_TYPELIB_PATH=${GI_TYPELIB_PATH_STR}" "ZONEINFO_DIRECTORY=${PROJECT_SOURCE_DIR}/zoneinfo")
  if(APPLE)
    list(APPEND test_env "DYLD_LIBRARY_PATH=${LIBRARY_PATH_STR}")
  elseif(NOT MSVC) #then UNIX-like
    list(APPEND test_env "LD_LIBRARY_PATH=${LIBRARY_PATH_STR}")
  endif()

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
