find_package( FFmpeg REQUIRED )

set( Omega_Impl_Headers
   )

set( Omega_Impl_Srcs
     Codec.cpp
     Context.cpp
     Conversion.cpp
     Decoder.cpp
   )

add_library( omega-impl STATIC ${Omega_Impl_Srcs} )
target_link_libraries     ( omega-impl PUBLIC  ${FFMPEG_LIBRARIES} -lswscale )
target_include_directories( omega-impl PRIVATE ${FFMPEG_INCLUDE_DIRS}        )
