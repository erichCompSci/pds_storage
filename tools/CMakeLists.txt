
ADD_EXECUTABLE(pdsload pdsload.cc) 
ADD_EXECUTABLE(pdslookup pdslookup.c) 
TARGET_LINK_LIBRARIES(pdsload pdsagent ${BASE_LIBS})
TARGET_LINK_LIBRARIES(pdslookup pdsagent ${BASE_LIBS})

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/pdsload ${CMAKE_CURRENT_BINARY_DIR}/pdslookup DESTINATION bin)

