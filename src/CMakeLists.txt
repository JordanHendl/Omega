add_subdirectory( impl  )
add_subdirectory( tests )

find_package( FFmpeg REQUIRED )

set( Omega_Headers
     Omega.h
     Structures.h
     Loader.h
     Saver.h
     Timer.h
   )

set( Omega_Srcs
     Loader.cpp
     Saver.cpp
     Timer.cpp
     Structures.cpp
   )

add_library( omega SHARED ${Omega_Srcs} )
target_link_libraries     ( omega PUBLIC  omega-impl             )
target_include_directories( omega PRIVATE ${FFMPEG_INCLUDE_DIRS} )
