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

ADD_EXECUTABLE(basic basic.c cmdline.c) 
ADD_EXECUTABLE(basica basic2.c)
ADD_EXECUTABLE(regression regression.cc cmdline.c)
ADD_EXECUTABLE(bulkload bulkload.c)
ADD_EXECUTABLE(sinktest sinktest.c)
TARGET_LINK_LIBRARIES(basic pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(basica pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(bulkload pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(sinktest pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(regression pdsagent ${BASE_LIBS})

