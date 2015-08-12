cmake_minimum_required(VERSION 2.8.3)
cmake_policy(VERSION 2.8.3)
project(ffs)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

MESSAGE(STATUS "Looking for GNU gengetopt")
FIND_PROGRAM(GENGETOPT gengetopt
  HINTS "/opt/local/bin" "/usr/local/bin")

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(CheckStructHasMember)
include(CreateLibtoolFile)
include(AddLibtoolLibrary)
include(FindCERCSProject)
include(CERCS_package)
include(ExternalProject)

CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("float" SIZEOF_FLOAT)
CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("off_t" SIZEOF_OFF_T)
CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)

IF(NOT DEFINED BUILD_SHARED_STATIC)
  SET( BUILD_SHARED_STATIC "BOTH")
ENDIF(NOT DEFINED BUILD_SHARED_STATIC)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

set (CMAKE_MACOSX_RPATH 1)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/common ${CMAKE_CURRENT_SOURCE_DIR}/agent
${CMAKE_CURRENT_SOURCE_DIR}/engine ${CMAKE_CURRENT_BINARY_DIR}/server
${CMAKE_CURRENT_SOURCE_DIR}/tests)

if (DEFINED CERCS_USE_INSTALLED) 
   set (CERCS_USE_INSTALLED "USE_INSTALLED")
endif(DEFINED CERCS_USE_INSTALLED) 

set (COMMON_SRC_LIST formats.c trace.c logging.c)
FOREACH (COMMON_SRC ${COMMON_SRC_LIST})
    LIST (APPEND COMMON_MASTER_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/common/${COMMON_SRC})
ENDFOREACH()

set (AGENT_SRC_LIST client.c client_entity.c client_hl.c)
FOREACH (AGENT_SRC ${AGENT_SRC_LIST})
    LIST (APPEND AGENT_MASTER_SRC_LIST agent/${AGENT_SRC})
ENDFOREACH()

set (ENGINE_SRC_LIST Domain.cc Entity.cc Context.cc objectId.cc
Proactive.cc ContextBindable.cc utils.c EventWrapper.cc CleaningThread.cc
EngineGuard.cc base64.c storage_stone.c)
FOREACH (ENGINE_SRC ${ENGINE_SRC_LIST})
    LIST (APPEND ENGINE_MASTER_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/engine/${ENGINE_SRC})
ENDFOREACH()




IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

set (PDSAGENT_MASTER_SRC_LIST ${COMMON_MASTER_SRC_LIST} ${AGENT_MASTER_SRC_LIST})


FIND_CERCS_PROJECT (cercs_env LIBRARY cercs_env INCLUDES cercs_env.h REQUIRED ${CERCS_USE_INSTALLED})
FIND_CERCS_PROJECT (atl LIBRARY atl INCLUDES atl.h REQUIRED ${CERCS_USE_INSTALLED})
FIND_CERCS_PROJECT (ffs LIBRARY ffs INCLUDES ffs.h REQUIRED ${CERCS_USE_INSTALLED})
FIND_CERCS_PROJECT (cmrpc LIBRARY cmrpc INCLUDES cmrpc.h REQUIRED ${CERCS_USE_INSTALLED})
FIND_CERCS_PROJECT (evpath LIBRARY evpath INCLUDES evpath.h REQUIRED ${CERCS_USE_INSTALLED})
FIND_CERCS_PROJECT(dill LIBRARY dill INCLUDES dill.h ${CERCS_USE_INSTALLED})
FIND_PACKAGE(LibXml2)
if (NOT ${LIBXML2_FOUND}) 
   unset (LIBXML2_FOUND)
   MESSAGE (STATUS "LibXML2 not found, will download and build)")
   MESSAGE (STATUS "   YOU MUST HAVE WRITE PERMISSIONS TO THE CMAKE_INSTALL_PREFIX DIRECTORY OR THE BUILD WILL FAIL!")
   set(libxml2_release "2.9")
   set(libxml2_patch_version 0)
   set(libxml2_url "ftp://xmlsoft.org/libxml2/libxml2-sources-${libxml2_release}.${libxml2_patch_version}.tar.gz")
   set(libxml2_md5 "7da7af8f62e111497d5a2b61d01bd811")

   ExternalProject_Add(project_libxml2
     DEPENDS ${_XML2_DEPENDS}
     URL ${libxml2_url}
     URL_MD5 ${libxml2_md5}
     INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}
     BUILD_IN_SOURCE 1
     CONFIGURE_COMMAND ./configure --without-python --prefix=${CMAKE_INSTALL_PREFIX}
     BUILD_COMMAND make
     INSTALL_COMMAND make install
   )
   set(  LIBXML2_LIBRARIES libxml2)
   include_directories(${CMAKE_INSTALL_PREFIX}/include/)

   ExternalProject_Get_Property(project_libxml2 install_dir)


   add_library(libxml2 STATIC IMPORTED)
   set_property(TARGET libxml2 PROPERTY IMPORTED_LOCATION
       ${install_dir}/lib/libxml2.a)
   add_dependencies(libxml2 project_libxml2)

endif()
FIND_PACKAGE (Threads)

list (APPEND INC_DIRS ${EVPATH_INCLUDE_DIR} ${CERCS_ENV_INCLUDE_DIR} ${ATL_INCLUDE_DIR} ${FFS_INCLUDE_DIR} ${CMRPC_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
list (APPEND LIB_DIRS ${EVPATH_LIB_DIR} ${CERCS_ENV_LIB_DIR} ${ATL_LIB_DIR} ${FFS_LIB_DIR} ${CMRPC_LIB_DIR})
list (APPEND BASE_LIBS ${EVPATH_LIBRARIES} ${CERCS_ENV_LIBRARIES} ${ATL_LIBRARIES} ${CMRPC_LIBRARIES} ${FFS_LIBRARIES} ${DL_LIBRARIES} ${LIBXML2_LIBRARIES})

find_library(M_LIB m)
if (${M_LIB})
  list (APPEND BASE_LIBS "-lm")
endif()

list (APPEND DEPLIBS "-L${ATL_LIB_DIR}" "${ATL_LIB_DIR}/libatl.la")
list (APPEND DEPLIBS "-L${CERCS_ENV_LIB_DIR}" "${CERCS_ENV_LIB_DIR}/libcercs_env.la")

foreach(arg ${DEPLIBS})
   set(DEP_STRING "${DEP_STRING} ${arg}")
endforeach(arg ${DEPLIBS})

add_libtool_library(NAME pdsagent SRC_LIST ${PDSAGENT_MASTER_SRC_LIST} DEP_LIBS "${DEP_STRING}" LINK_LIBS ${BASE_LIBS})

INCLUDE_DIRECTORIES(${INC_DIRS})
LINK_DIRECTORIES(${LIB_DIRS})

if (NOT DEFINED (PDS_SERVER_PORT))
   set(PDS_SERVER_PORT 8848)
endif()
if (NOT DEFINED (PDS_SERVICE_DOMAIN))
  set(PDS_SERVICE_DOMAIN "gatech.edu")
endif()


CHECK_INCLUDE_FILE(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILE(stdarg.h HAVE_STDARG_H)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE(sys/times.h HAVE_SYS_TIMES_H)
CHECK_INCLUDE_FILE(sys/uio.h HAVE_SYS_UIO_H)

configure_file(CTestCustom.ctest.in CTestCustom.ctest @ONLY)

CREATE_LIBTOOL_FILE(pdsagent /lib)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/agent/pds.h DESTINATION include)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/agent/pdsdefs.h DESTINATION include)
INSTALL(TARGETS ${INSTALL_TARGETS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

if (${CMAKE_C_COMPILER_ID} MATCHES "Intel") 
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -shared-intel")
endif()

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}"  (options are: Debug,Release,RelWithDebInfo,MinSizeRel))
MESSAGE( STATUS "BUILD_SHARED_STATIC = ${BUILD_SHARED_STATIC}")
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "    to use installed CERCS libraries specify -DCERCS_USE_INSTALLED=1" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )

INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/dump_dir)
ADD_CUSTOM_TARGET( make_dump_directory ALL
                  COMMAND ${CMAKE_COMMAND} -E make_directory
                  ${CMAKE_CURRENT_BINARY_DIR}/dump_dir )
ENDIF()

#get_filename_component(single_mod
#                       ./tests/experiments/single_modifier_per_node.sh
#                       ABSOLUTE)
#
#get_filename_component(clean_up
#                       ./tests/experiments/clean_up_all.sh
#                       ABSOLUTE)
#
#ADD_CUSTOM_TARGET( copy_experiments_to_build ALL
#                   COMMAND ${CMAKE_COMMAND} -E copy 
#                   ${single_mod} ${CMAKE_CURRENT_BINARY_DIR}/tests/experiments/single_modifier_per_node.sh)
#
#ADD_CUSTOM_COMMAND( TARGET copy_experiments_to_build
#                    POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy
#                   ${clean_up} ${CMAKE_CURRENT_BINARY_DIR}/tests/experiments/clean_up_all.sh )

ADD_SUBDIRECTORY( jni )
ADD_SUBDIRECTORY( server )
ADD_SUBDIRECTORY( tools )
ADD_SUBDIRECTORY( tests )
