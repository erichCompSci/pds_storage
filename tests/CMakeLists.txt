cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

IF ((GENGETOPT STREQUAL "GENGETOPT-NOTFOUND") OR (GENGETOPT STREQUAL ""))
  MESSAGE (AUTHOR_WARNING "Didn't find gengetopt.  Using pre-generated cmdline.h and cmdline.c.  If you modify tests/cmdline.ggo, changes will *not* be reflected in executable.")
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cmdline.c ${CMAKE_CURRENT_BINARY_DIR}/cmdline.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c.pregen ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h.pregen
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c.pregen ${CMAKE_CURRENT_BINARY_DIR}/cmdline.c
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h.pregen ${CMAKE_CURRENT_BINARY_DIR}/cmdline.h)
ELSE ()
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cmdline.c ${CMAKE_CURRENT_BINARY_DIR}/cmdline.h
    COMMAND ${GENGETOPT} --input=${CMAKE_CURRENT_SOURCE_DIR}/cmdline.ggo
    --output-dir=${CMAKE_CURRENT_BINARY_DIR}
    --unamed-opts
    --conf-parser
    --include-getopt
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/cmdline.c ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c.pregen
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/cmdline.h ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h.pregen
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.ggo
    )
ENDIF()

SET (LOCAL_DEPS cmdline.c logging.c)

ADD_EXECUTABLE(basic basic.c ${LOCAL_DEPS}) 
ADD_EXECUTABLE(event_channel_test_recv event_channel_test_recv.c ${LOCAL_DEPS})
ADD_EXECUTABLE(event_channel_test_send event_channel_test_send.c ${LOCAL_DEPS})
ADD_EXECUTABLE(context_channel_test_recv context_channel_test_recv.c ${LOCAL_DEPS})
ADD_EXECUTABLE(context_channel_test_send context_channel_test_send.c ${LOCAL_DEPS})
ADD_EXECUTABLE(storage_stone_recv storage_stone_recv.c ${LOCAL_DEPS})
ADD_EXECUTABLE(storage_stone_send storage_stone_send.c ${LOCAL_DEPS})
TARGET_LINK_LIBRARIES(basic pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(event_channel_test_recv pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(event_channel_test_send pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(context_channel_test_recv pdsagent  ${BASE_LIBS})
TARGET_LINK_LIBRARIES(context_channel_test_send pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(storage_stone_recv pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(storage_stone_send pdsagent ${BASE_LIBS})

add_subdirectory( experiments )
