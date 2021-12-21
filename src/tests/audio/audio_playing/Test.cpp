#include <Omega.h>
#include <iostream>
#include <Hathor/Hathor.h>
#include <Hathor/Structures.h>
#include <sstream>
#include <thread>
#include <chrono>

auto tmp_vec = std::vector<unsigned short>() ;

static auto audioCallback( const omega::Audio& sound )  -> void 
{
  if( sound.valid() )
  {
    for( auto& sh : sound.data ) tmp_vec.push_back( sh ) ;
  }
}

int main( int argc, const char* argv[] )
{
  if( argc != 2 ) { std::cout << "Usage: " << argv[ 0 ] << " <video_file>" << std::endl ; return 0 ; }
 
  auto cfg = omega::LoaderConfig() ;
  cfg.path = argv[ 1 ] ;
  cfg.wait_for_next_frame = false ;
  auto cbs = omega::Callbacks() ;
  cbs.add( &audioCallback ) ;
  omega::Loader loader( cfg, cbs ) ;
  
  std::cout << loader.info() << std::endl ;
  
  hathor::Library::initialize() ;
  loader.play() ;
  while( loader.playing() ) {} ;
  
  hathor::Audio audio ;
  audio.initialize( tmp_vec.data(), tmp_vec.size(), loader.frequency(), hathor::Format::Stereo16 ) ;
  audio.play() ;
  audio.wait() ;
  return 0 ;
}