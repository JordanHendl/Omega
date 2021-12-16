CMAKE_MINIMUM_REQUIRED( VERSION 3.8.0 ) 

LIST( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
INCLUDE( CompileGLSL )
PROJECT( Omega CXX )

OPTION( BUILD_TESTS   "Whether or not to build tests for this project."        ON  )
OPTION( BUILD_RELEASE "Whether or not to build binary tools for this project." OFF )

MESSAGE( "${CMAKE_PROJECT_NAME} Prebuild" )
MESSAGE( ""                               )

# Set build config.
SET( ARCHITECTURE      "64bit" CACHE STRING "The system architecture."                     )
SET( CXX_STANDARD      "17"    CACHE STRING "The C++ standard to use for building."        )
SET( MAJOR             "0"     CACHE STRING "The major version of this build."             )
SET( MINOR             "0"     CACHE STRING "The minor version of this build."             )
SET( BRANCH            "0"     CACHE STRING "The branch version of this build."            )
SET( REVISION          "0"     CACHE STRING "The revision of this build."                  )
SET( GENERATOR         "DEB"   CACHE STRING "The Package Generator to use for this build." )

SET( PROJECT_VERSION "${MAJOR}.${MINOR}.${BRANCH}.${REVISION}" )

SET(CMAKE_CXX_STANDARD          17 )
SET(CMAKE_CXX_STANDARD_REQUIRED ON )

if( BUILD_RELEASE )
  if( MSVC )
    set( COMPILE_OPTIONS /W2 /O2 ) 
  elseif( UNIX )
    set( COMPILE_OPTIONS -Wall -Wextra -Wpedantic -fPIC -O2 ) 
  endif()
else()
  if( MSVC )
    set( COMPILE_OPTIONS /W2 /DEBUG ) 
  elseif( UNIX )
    set( COMPILE_OPTIONS -Wall -Wextra -Wpedantic -fPIC -g ) 
  endif()
endif()

add_compile_options( ${COMPILE_OPTIONS} )

IF( WIN32 )
  SET( CMAKE_PREFIX_PATH "C:\\Program Files\\" CACHE STRING "The default path to look for dependancies." )
ELSEIF( UNIX AND NOT APPLE )
  SET( CMAKE_PREFIX_PATH "/usr/local/Catalyst" CACHE STRING "The default path to look for dependancies." )
ENDIF()


SET( EXPORT_BIN_DIR     bin     )
SET( EXPORT_INCLUDE_DIR include )
SET( EXPORT_LIB_DIR     lib     )
SET( EXPORT_DOC_DIR     doc     )

# Set output directories.
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXPORT_LIB_DIR} )
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXPORT_LIB_DIR} )
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXPORT_BIN_DIR} )
SET(DOCUMENTATION_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXPORT_DOC_DIR} )

set( CONFIG_DIR         ${CMAKE_PROJECT_NAME}                       )
set( DOXYGEN_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/doc             )
set( BUILD_DIR          ${CMAKE_BINARY_DIR}                         )
set( HEADER_INSTALL_DIR ${EXPORT_INCLUDE_DIR}/${CMAKE_PROJECT_NAME} )
SET( SHADER_DIR         ${CMAKE_BINARY_DIR}/shader_headers/         )

include( Package )

include_directories( src           )
make_directory     ( ${SHADER_DIR} )
include_directories( ${SHADER_DIR} )

add_subdirectory( external )
add_subdirectory( src )

#install( EXPORT Omega FILE OmegaConfig.cmake 
#                    DESTINATION cmake 
#                    NAMESPACE   omega::
#                    COMPONENT   devel )
