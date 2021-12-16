#include <Omega.h>
#include <iostream>
#include <CatalystEX/Loaders/Image.h>
#include <sstream>

int main( int argc, const char* argv[] )
{
  if( argc != 2 ) { std::cout << "Usage: " << argv[ 0 ] << " <video_file>" << std::endl ; return 0 ; }
  
  auto cfg = omega::LoaderConfig() ;
  cfg.path = argv[ 1 ] ;
  
  omega::Loader loader( cfg ) ;
  
//  std::cout << loader.info() << std::endl ;
  
  for( int index = 0; index < 50; index++ )
  {
    std::stringstream str ;
    
    loader.advance() ;
    auto frame = loader.frame() ;
    auto info = cata::ex::ImageInfo<unsigned char>() ;
    
    str << "out_" << index << ".png" ;
    info.pixels   = frame.pixels   ;
    info.width    = frame.width    ;
    info.height   = frame.height   ;
    info.channels = frame.channels ;
    
    cata::ex::savePNG( info, str.str().c_str() ) ;
    
    std::cout << "Frame " << index << " width: " << info.width << " height " << info.height << " channels " << info.channels << std::endl ;
  }
  
  return 0 ;
}