find_package( Catalyst   REQUIRED )
find_package( CatalystEX REQUIRED )
find_package( Hathor     REQUIRED )

add_executable       ( test_audio_playing Test.cpp )
target_link_libraries( test_audio_playing PRIVATE omega cata::vulkan cata::window excata::loaders hathor::hathor )