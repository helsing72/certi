add_subdirectory(libgraphc)
add_subdirectory(Billard)
add_subdirectory(utility)
add_subdirectory(mom_explorer)
add_subdirectory(ModernBillard)
add_subdirectory(TimeMaster)
add_subdirectory(testFederate)

# bug #55104 Test if rpc include directory is available
# reason is Fedora removed (in v28) the rpc header from the standard glibc
set(RPC_DEFAULT_INCLUDE_PATHS
    ${CMAKE_CURRENT_LIST_DIR}/..     
    /usr/local/include
    /usr/include
    /usr/include/tirpc
    /opt/include
    )
    
set(RPC_DEFAULT_LIB_PATHS
    ${CMAKE_CURRENT_LIST_DIR}/..     
    /usr/local/lib
    /usr/local/lib64
    /usr/lib
    /usr/lib64
    /opt/
    )

FIND_PATH(RPC_INCLUDE_DIR
	NAMES "rpc/rpc.h"
	HINTS /usr/include/tirpc
	PATHS ${RPC_DEFAULT_INCLUDE_PATHS}
	)
	
FIND_LIBRARY(RPCLIB_LIBS
    NAMES librpc rpclib rpc tirpc
	PATHS ${RPC_DEFAULT_LIB_PATHS}
	)
	
if(NOT RPC_INCLUDE_DIR)
    MESSAGE("Could not find RPC Headers, certiCheckHostAndIP test won't be compiled!")
else()
	MESSAGE("RPC Headers found in ${RPC_INCLUDE_DIR}")
	INCLUDE_DIRECTORIES(${RPC_INCLUDE_DIR})
endif()

if(NOT RPCLIB_LIBS)
    MESSAGE("Could not find RPC Libraries, certiCheckHostAndIP test won't be compiled!")
else()
	MESSAGE("RPC Libraries found in found in ${RPCLIB_LIBS}")
endif()

# Do not compile this on Win32 (not very useful)
if ((NOT WIN32) AND (RPC_INCLUDE_DIR) AND (RPCLIB_LIBS))
   if (CMAKE_SYSTEM_NAME MATCHES "Linux")
      set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/certiCheckHostAndIP.c PROPERTIES COMPILE_DEFINITIONS "LINUX")
   endif()
   add_executable(CertiCheckHostAndIP certiCheckHostAndIP.c)
  target_link_libraries(CertiCheckHostAndIP ${RPCLIB_LIBS})
   install(TARGETS CertiCheckHostAndIP
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)
endif((NOT WIN32) AND (RPC_INCLUDE_DIR) AND (RPCLIB_LIBS))

