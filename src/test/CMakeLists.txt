include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_DATADIR "\\\"${CMAKE_SOURCE_DIR}/test-data\\\"")
add_definitions(-DTEST_DATADIR=${TEST_DATADIR})

########### next target ###############

set(copycluster_SRCS copycluster.c)

add_executable(copycluster ${copycluster_SRCS})

target_link_libraries(copycluster ical icalss)

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

add_executable(regression ${regression_SRCS})

target_link_libraries(regression ical icalss icalvcal)

add_test(NAME regression WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin COMMAND regression)

########### next target ###############

set(parser_SRCS icaltestparser.c)

add_executable(parser ${parser_SRCS})

target_link_libraries(parser ical icalss)

########### next target ###############

if(NOT WIN32)
  set(STOW_SRCS stow.c)

  add_executable(stow ${STOW_SRCS})

  target_link_libraries(stow ical icalss)
endif(NOT WIN32)

########### next target ###############

set(recur_SRCS recur.c)

add_executable(recur ${recur_SRCS})

target_link_libraries(recur ical icalss)

########### next target ###############

if(HAVE_UNISTD_H)
  set(testmime_SRCS testmime.c)

  add_executable(testmime ${testmime_SRCS})

  target_link_libraries(testmime ical icalss)
endif(HAVE_UNISTD_H)

########### next target ###############

set(testvcal_SRCS testvcal.c)

add_executable(testvcal ${testvcal_SRCS})

target_link_libraries(testvcal ical icalss icalvcal)

########### next target ###############

set(process_SRCS process.c)

add_executable(process ${process_SRCS})

target_link_libraries(process ical icalss)

########### next target ###############

if(NOT WIN32)
  set(timezones_SRCS timezones.c)

  add_executable(timezones ${timezones_SRCS})

  target_link_libraries(timezones ical icalss)

  add_test(NAME timezones WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin COMMAND timezones)
endif(NOT WIN32)

########### install files ###############

