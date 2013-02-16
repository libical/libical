include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/libical)

########### next target ###############

set(client_SRCS client.c)

add_executable(client ${client_SRCS})

target_link_libraries(client)

########### install files ###############

install(FILES
  icalcap.h
  icalcap_impl.h
  icalcap_message_impl.h
  icalcap_server.h
  icalcap_server_impl.h
  icalcap_session.h
  icalcap_session_impl.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/libicalcap
)
