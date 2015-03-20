# setup CPack

include(${CMAKE_SOURCE_DIR}/CMake/InstallSupportLibraries.cmake)

configure_file( "${CMAKE_SOURCE_DIR}/DrosophilaOmmatidiaExplorerCPackOptions.cmake.in"
    "${CMAKE_BINARY_DIR}/DrosophilaOmmatidiaExplorerCPackOptions.cmake" @ONLY 
  )

set( CPACK_PROJECT_CONFIG_FILE 
  "${CMAKE_BINARY_DIR}/DrosophilaOmmatidiaExplorerCPackOptions.cmake"
  )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY 
  "DrosophilaOmmatidiaExplorer is a ..." 
  )
set( CPACK_PACKAGE_VENDOR "Hatini Lab, Sackler School of Graduate Biomedical Sciences, Tufts University" )
set( CPACK_PACKAGE_CONTACT "rodrigo.cilla@tufts.edu" )
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/ReadMe.rst")
#set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/Licenses/Copyright.txt")
#set( CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/CPack.GenericWelcome.txt" )
set( CPACK_PACKAGE_VERSION_MAJOR 2015 )
set( CPACK_PACKAGE_VERSION_MINOR 04 )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "DrosophilaOmmatidiaExplorer" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "DrosophilaOmmatidiaExplorer-2015.04" )

# taken from CMake
if( NOT DEFINED CPACK_SYSTEM_NAME )
  # make sure package is not Cygwin-unknown, for Cygwin just
  # cygwin is good for the system name
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
    set( CPACK_SYSTEM_NAME Cygwin )
  else("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
    if( CMAKE_SYSTEM_PROCESSOR MATCHES "unknown" )
      exec_program( uname ARGS "-m" OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR )
    endif( CMAKE_SYSTEM_PROCESSOR MATCHES "unknown" )

    set( CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR} )
  endif("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
endif( NOT DEFINED CPACK_SYSTEM_NAME )

#set up the package name on mac
if( ${CPACK_SYSTEM_NAME} MATCHES Darwin )
  #use uname -m instead of uname -p
  #execute_process( COMMAND uname -m
  #                 OUTPUT_VARIABLE PLATFORM)
  #remove end of line character
  #string(REGEX REPLACE "/n" "" PLATFORM2 ${PLATFORM})
  #set the system name
  if( CMAKE_OSX_ARCHITECTURES )
    set( CPACK_SYSTEM_NAME Darwin-${CMAKE_OSX_ARCHITECTURES} )
  endif( CMAKE_OSX_ARCHITECTURES )
endif( ${CPACK_SYSTEM_NAME} MATCHES Darwin )

#set up the package name on windows
if( ${CPACK_SYSTEM_NAME} MATCHES Windows )
 if( CMAKE_CL_64 )
   set( CPACK_SYSTEM_NAME win64-${CMAKE_SYSTEM_PROCESSOR} )
 else( CMAKE_CL_64 )
   set( CPACK_SYSTEM_NAME win32-${CMAKE_SYSTEM_PROCESSOR} )
 endif( CMAKE_CL_64 )
endif( ${CPACK_SYSTEM_NAME} MATCHES Windows )

if( NOT DEFINED CPACK_PACKAGE_FILE_NAME )
  # if the CPACK_PACKAGE_FILE_NAME is not defined by the cache
  # default to source package - system, on cygwin system is not 
  # needed
  if( CYGWIN )
    set( CPACK_PACKAGE_FILE_NAME 
      "${CPACK_SOURCE_PACKAGE_FILE_NAME}" )
  else( CYGWIN )
    set( CPACK_PACKAGE_FILE_NAME
      "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${CPACK_SYSTEM_NAME}" )
  endif( CYGWIN )
endif( NOT DEFINED CPACK_PACKAGE_FILE_NAME )

if( WIN32 AND NOT UNIX )
  include( ${CPACK_PROJECT_CONFIG_FILE} )
else( WIN32 AND NOT UNIX )
  set( CPACK_STRIP_FILES "bin/DrosophilaOmmatidiaExplorer" )
  set( CPACK_SOURCE_STRIP_FILES "" )
endif( WIN32 AND NOT UNIX )

set( CPACK_PACKAGE_EXECUTABLES "bin/DrosophilaOmmatidiaExplorer" "DrosophilaOmmatidiaExplorer" )

# # # # cygwin specific packaging stuff
# # if(CYGWIN)
# # endif( CYGWIN )

if( WIN32 )
	install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} 
		DESTINATION bin COMPONENT Runtime )
endif( WIN32 )

set( CPACK_COMPONENTS_ALL
  Runtime
#  Libraries
#  headers
)

include( CPack )
