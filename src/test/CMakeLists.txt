include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_DATADIR "\\\"${CMAKE_SOURCE_DIR}/test-data\\\"")
add_definitions(-DTEST_DATADIR=${TEST_DATADIR})

macro(setprops _name)
  if(UNIX)
    set_tests_properties(${_name} PROPERTIES
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
  endif()
  if(WIN32)
    set_tests_properties(${_name} PROPERTIES
      ENVIRONMENT "PATH=${EXECUTABLE_OUTPUT_PATH};${CMAKE_BINARY_DIR}/bin;%PATH%"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
  endif()
  if(CYGWIN OR MSYS)
    set_tests_properties(${_name} PROPERTIES
      ENVIRONMENT "PATH=${EXECUTABLE_OUTPUT_PATH}:${CMAKE_BINARY_DIR}/lib:$ENV{PATH}"
    )
  endif()
endmacro()

macro(testme _name _srcs)
  add_executable(${_name} ${_srcs})
  target_link_libraries(${_name} ical icalss icalvcal)
  add_test(NAME ${_name} COMMAND ${_name})
  setprops(${_name})
endmacro()

########### next target ###############

#not sure how to run this as it takes 2 cluster files as command line args
#set(copycluster_SRCS copycluster.c)
#add_test(copycluster "${copycluster_SRCS}")

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
testme(regression "${regression_SRCS}")

########### next target ###############

#not sure how to use this test
#set(parser_SRCS icaltestparser.c)
#testme(parser "${parser_SRCS}")

########### next target ###############

#not sure how to use this test
#set(stow_SRCS stow.c)
#testme(stow "${stow_SRCS}")

########### next target ###############

set(recur_SRCS recur.c)
testme(recur "${recur_SRCS}")

########### next target ###############

if(NOT WIN32) #since we currently do not have a Windows reference file
  if(HAVE_UNISTD_H) #getopt is required
    set(icalrecurtest_SRCS icalrecur_test.c)
    add_executable(icalrecurtest ${icalrecurtest_SRCS})
    target_link_libraries(icalrecurtest ical icalss icalvcal)
    set(test_cmd "${CMAKE_BINARY_DIR}/src/test/icalrecurtest${CMAKE_EXECUTABLE_SUFFIX}")
    set(test_args "")
    add_test(NAME icalrecurtest
      COMMAND ${CMAKE_COMMAND}
      -D test_cmd=${test_cmd}
      -D test_args:string=${test_args}
      -D output_blessed=${CMAKE_SOURCE_DIR}/src/test/icalrecur_test.out
      -D output_test=${CMAKE_BINARY_DIR}/bin/test.out
      -P ${CMAKE_SOURCE_DIR}/cmake/run_test.cmake
    )
    setprops(icalrecurtest)
  endif()
endif()

########### next target ###############

if(HAVE_UNISTD_H) #getopt is required
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

if(NOT USE_BUILTIN_TZDATA)
  set(timezones_SRCS timezones.c)
  testme(timezones "${timezones_SRCS}")
endif()

########### install files ###############

