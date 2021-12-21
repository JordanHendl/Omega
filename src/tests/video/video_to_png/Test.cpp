#include <Omega.h>
#include <iostream>
#include <CatalystEX/Loaders/Image.h>
#include <sstream>

int main( int argc, const char* argv[] )
{
  if( argc != 2 ) { std::cout << "Usage: " << argv[ 0 ] << " <video_file>" << std::endl ; return 0 ; }
  
  auto cfg = omega::LoaderConfig() ;
  cfg.path = argv[ 1 ] ;
  
  auto cbs = omega::Callbacks() ;
  omega::Loader loader( cfg, cbs ) ;
  
  std::cout << loader.info() << std::endl ;
  
  for( int index = 0; index < 50; index++ )
  {
    std::stringstream str ;
    
    loader.advance() ;
  }
  
  return 0 ;
}