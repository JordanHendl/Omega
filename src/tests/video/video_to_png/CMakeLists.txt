find_package( Catalyst   REQUIRED )
find_package( CatalystEX REQUIRED )

add_executable       ( test_video_to_png Test.cpp )
target_link_libraries( test_video_to_png PRIVATE omega excata::loaders )