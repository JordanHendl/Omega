SET( EXT_FFMPEG_VER       1.0                                   )
SET( EXT_FFMPEG_REL       0                                     )
SET( EXT_FFMPEG_REL_VER   ${EXT_FFMPEG_VEW}.${EXT_FFMPEG_REL} )

SET( EXT_FFMPEG_TAR_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/assimp-4.0.1.tar.gz             ) 
SET( EXT_FFMPEG_UNTAR_DIR ${CMAKE_BINARY_DIR}/ext/ffmpeg/src/                         )
SET( EXT_FFMPEG_BLD_DIR   ${CMAKE_BINARY_DIR}/ext/ffmpeg/build                        )
SET( EXT_FFMPEG_SRC_DIR   ${CMAKE_BINARY_DIR}/ext/ffmpeg/src/assimp-4.0.1/            )
SET( EXT_FFMPEG_CMAKE_DIR ${EXT_FFMPEG_SRC_DIR}                                       )
SET( EXT_FFMPEG_INCS      ${EXT_FFMPEG_SRC_DIR}/include ${EXT_FFMPEG_BLD_DIR}/include )

IF( WIN32 )
  SET( EXT_FFMPEG_LIB     ${EXT_FFMPEG_BLD_DIR}/Release/ffmpeg.lib                                          )
  SET( EXT_FFMPEG_LIBS    ${EXT_FFMPEG_BLD_DIR}/Release/ffmpeg.dll ${EXT_FFMPEG_BLD_DIR}/Release/ffmpeg.lib )
ENDIF()

IF( UNIX )
  SET( EXT_FFMPEG_LIBS ${EXT_FFMPEG_BLD_DIR}/code/libassimp.so
                       ${EXT_FFMPEG_BLD_DIR}/code/libassimp.so.4
                       ${EXT_FFMPEG_BLD_DIR}/code/libassimp.so.4.0.1
#                       ${EXT_FFMPEG_BLD_DIR}/contrib/zlib/
     )

  SET( EXT_FFMPEG_LIB  ${EXT_FFMPEG_BLD_DIR}/bin/libassimp.so
     )
ENDIF()

FILE( MAKE_DIRECTORY ${EXT_FFMPEG_BLD_DIR} )
FILE( MAKE_DIRECTORY ${EXT_FFMPEG_SRC_DIR} )

MESSAGE( "Untarring Assimp library source" )
EXECUTE_PROCESS( 
  COMMAND tar -xf ${EXT_FFMPEG_TAR_DIR}
  WORKING_DIRECTORY ${EXT_FFMPEG_UNTAR_DIR}
  )

EXECUTE_PROCESS( 
  COMMAND ${CMAKE_COMMAND} ${EXT_FFMPEG_CMAKE_DIR}
  WORKING_DIRECTORY ${EXT_FFMPEG_BLD_DIR}
  )

IF( WIN32 )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_FFMPEG_LIBS}
    COMMAND MSBuild ASSIMP2.sln /property:Configuration=Release -maxcpucount:7
    WORKING_DIRECTORY ${EXT_FFMPEG_BLD_DIR}
    )
ENDIF()

IF( UNIX )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_FFMPEG_LIBS}
    COMMAND $(MAKE)
    WORKING_DIRECTORY ${EXT_FFMPEG_BLD_DIR}
    )
ENDIF()

ADD_CUSTOM_TARGET(
  EXT_Assimp ALL
  DEPENDS ${EXT_FFMPEG_LIBS}
)

SET( Assimp_LIBRARIES    ${EXT_FFMPEG_LIBS} CACHE INTERNAL "ASSIMP Libaries"     )
SET( Assimp_INCLUDE_DIRS ${EXT_FFMPEG_INCS} CACHE INTERNAL "ASSIMP Include Dirs" )

INSTALL( FILES ${EXT_FFMPEG_LIBS} DESTINATION ${EXPORT_LIB_DIR} COMPONENT tools )
