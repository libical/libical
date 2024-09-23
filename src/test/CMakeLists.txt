include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/libical
  ${PROJECT_BINARY_DIR}/src/libical
  ${PROJECT_SOURCE_DIR}/src/libicalss
)

if(ICU_FOUND)
  include_directories(${ICU_INCLUDE_DIRS})
endif()

if(BDB_FOUND)
  include_directories(${BDB_INCLUDE_DIR})
endif()

set(TEST_DATADIR "\"${PROJECT_SOURCE_DIR}/test-data\"")
add_definitions(-DTEST_DATADIR=${TEST_DATADIR} -DTEST_ZONEDIR="${PROJECT_SOURCE_DIR}/zoneinfo")

macro(setprops _name)
  if(UNIX)
    set_tests_properties(${_name} PROPERTIES
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    )
  endif()
  if(WIN32)
    set(tpath "${PROJECT_BINARY_DIR}/bin;%PATH%")
    if(ICU_FOUND)
      set(tpath "${ICU_BINARY_DIR};${tpath}")
    endif()
    set_tests_properties(${_name} PROPERTIES
      ENVIRONMENT "PATH=${tpath}"
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    )
  endif()
  if(CYGWIN OR MSYS)
    set_tests_properties(${_name} PROPERTIES
      ENVIRONMENT "PATH=${EXECUTABLE_OUTPUT_PATH}:${PROJECT_BINARY_DIR}/lib:$ENV{PATH}"
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    )
  endif()
endmacro()

#macro to build (but do not create a test)
macro(buildme _name _srcs)
  add_executable(${_name} ${_srcs})
  add_dependencies(${_name} ical icalss icalvcal)
  if(NOT SHARED_ONLY)
    add_dependencies(${_name} ical-static icalss-static icalvcal-static)
  endif()
  target_link_libraries(${_name} ical icalss icalvcal)

  if(WITH_CXX_BINDINGS)
    add_dependencies(${_name} ical_cxx icalss_cxx)
    if(NOT SHARED_ONLY)
      add_dependencies(${_name} ical_cxx-static icalss_cxx-static)
    endif()
    target_link_libraries(${_name} ical_cxx icalss_cxx)
  endif()

  target_link_libraries(${_name} ${CMAKE_THREAD_LIBS_INIT})
  if(ICU_FOUND)
    target_link_libraries(${_name} ${ICU_LIBRARIES})
  endif()
  if(BDB_FOUND)
    target_link_libraries(${_name} ${BDB_LIBRARY})
  endif()
endmacro()

macro(testme _name _srcs)
  buildme(${_name} "${_srcs}")
  add_test(NAME ${_name} COMMAND ${_name})
  setprops(${_name})
endmacro()

########## libical-glib tests #########
if(ICAL_GLIB AND HAVE_INTROSPECTION)
  add_subdirectory(libical-glib)
endif()

########### next target ###############

set(copycluster_SRCS copycluster.c)
buildme(copycluster "${copycluster_SRCS}")

########### next target ###############

set(regression_SRCS
  regression.c
  regression.h
  regression-component.c
  regression-classify.c
  regression-utils.c
  regression-recur.c
  regression-storage.c
)
if(WITH_CXX_BINDINGS)
  list(APPEND regression_SRCS regression-cxx.cpp)
endif()
testme(regression "${regression_SRCS}")

########### next target ###############

set(parser_SRCS icaltestparser.c)
buildme(parser "${parser_SRCS}")

file(GLOB TEST_FILES ${PROJECT_SOURCE_DIR}/test-data/*.ics)
foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_test(NAME parser-${TEST_NAME} COMMAND parser ${TEST_FILE})
  setprops(parser-${TEST_NAME})
endforeach()

########### next target ###############

if(NOT WIN32 AND NOT ANDROID)
  #stow is very Unix specific
  set(stow_SRCS stow.c)
  buildme(stow "${stow_SRCS}")
endif()

########### next target ###############

set(recur_SRCS recur.c)
testme(recur "${recur_SRCS}")

########### next target ###############

if(HAVE_GETOPT) #getopt is required
  set(icalrecur_test_SRCS icalrecur_test.c)
  buildme(icalrecur_test "${icalrecur_test_SRCS}")
endif()

########### next target ###############
if(CMAKE_USE_PTHREADS_INIT)
  set(icaltm_test_SRCS icaltm_test.c)
  testme(icaltm_test "${icaltm_test_SRCS}")
endif()

########### next target ###############

if(HAVE_GETOPT) #getopt is required
  set(testmime_SRCS testmime.c)
  testme(testmime "${testmime_SRCS}")
endif()

########### next target ###############

set(testvcal_SRCS testvcal.c)
testme(testvcal "${testvcal_SRCS}")

########### next target ###############

set(process_SRCS process.c)
testme(process "${process_SRCS}")

########### next target ###############

if(NOT USE_BUILTIN_TZDATA AND NOT CYGWIN) #seeing >15% error rate on Cygwin. why?
  set(timezones_SRCS timezones.c)
  testme(timezones "${timezones_SRCS}")
endif()

########### next target ###############

set(builtin_timezones_SRCS builtin_timezones.c)
testme(builtin_timezones "${builtin_timezones_SRCS}")

########### next target ###############

#cmake v3.14 compare_files can ignore line endings => needed on Windows
set(CMAKE_COMPARE_FILES_IGNORE_EOL FALSE)
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.14)
  set(CMAKE_COMPARE_FILES_IGNORE_EOL TRUE)
endif()
if(NOT WIN32 OR CMAKE_COMPARE_FILES_IGNORE_EOL)
  if(NOT CYGWIN AND NOT USE_32BIT_TIME_T) #ignore_eol doesn't work on Cygwin. tests for years greater than 2037 will fail
    set(icalrecurtest_SRCS icalrecur_test.c)
    add_executable(icalrecurtest ${icalrecurtest_SRCS})
    target_link_libraries(icalrecurtest ical icalss icalvcal)
    if(WITH_CXX_BINDINGS)
      # https://github.com/libical/libical/pull/656
      set_property(TARGET icalrecurtest PROPERTY LINKER_LANGUAGE CXX)
    endif()
    set(test_cmd "${PROJECT_BINARY_DIR}/bin/icalrecurtest${CMAKE_EXECUTABLE_SUFFIX}")

    if(ICU_FOUND)
      #test rscale capable rrules
      set(test_args "-r")
      if(HAVE_ICU_DANGI)
        set(reference_data "icalrecur_withicu_dangi_test.out")
      else()
        set(reference_data "icalrecur_withicu_test.out")
      endif()
      add_test(NAME icalrecurtest-r
        COMMAND ${CMAKE_COMMAND}
        -D test_cmd=${test_cmd}
        -D test_args:string=${test_args}
        -D output_blessed=${PROJECT_SOURCE_DIR}/src/test/${reference_data}
        -D output_test=${PROJECT_BINARY_DIR}/bin/test.out
        -P ${PROJECT_SOURCE_DIR}/cmake/run_test.cmake
      )
      setprops(icalrecurtest-r)
    endif()

    #test non-rscale rrules
    set(test_args "")
    set(reference_data "icalrecur_test.out")
    add_test(NAME icalrecurtest
      COMMAND ${CMAKE_COMMAND}
      -D test_cmd=${test_cmd}
      -D test_args:string=${test_args}
      -D output_blessed=${PROJECT_SOURCE_DIR}/src/test/${reference_data}
      -D output_test=${PROJECT_BINARY_DIR}/bin/test.out
      -P ${PROJECT_SOURCE_DIR}/cmake/run_test.cmake
    )
    setprops(icalrecurtest)
  endif()
endif()

########### next target ###############

set(icalparser_ctrl_test_SRCS icalparser_ctrl_test.c)
testme(parser_ctrl "${icalparser_ctrl_test_SRCS}")

############# fuzzer tests ############

testme(icalcomponent_fuzz1 icalcomponent_fuzz1.c)
testme(icalcomponent_fuzz2 icalcomponent_fuzz2.c)

testme(icaltime_fuzz icaltime_fuzz.c)

option(LIBICAL_BUILD_TESTING_BIGFUZZ "Build big fuzzer tests." False)
if(LIBICAL_BUILD_TESTING_BIGFUZZ)
  foreach(
    fuzztest
    52787 # cpu timeout fails beyond 60s
    56171
    56539
    69341
    69344
    69527
    69888 # cpu timeout fails beyond 60s
    42536507 # cpu timeout fails beyond 60s
    usban
    usban2
    usban3
  )
    add_test(NAME fuzz1test${fuzztest} COMMAND icalcomponent_fuzz1 "${PROJECT_SOURCE_DIR}/test-data/fuzz${fuzztest}")
    setprops(fuzz1test${fuzztest})

    add_test(NAME fuzz2test${fuzztest} COMMAND icalcomponent_fuzz2 "${PROJECT_SOURCE_DIR}/test-data/fuzz${fuzztest}")
    setprops(fuzz2test${fuzztest})
  endforeach()
endif()
