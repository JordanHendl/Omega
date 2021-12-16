#include <Omega.h>
#include <iostream>
#include <Catalyst/vk/Vulkan.h>
#include <Catalyst/window/Window.h>
#include <Catalyst/window/Event.h>
#include <Catalyst/structures/Structures.h>
#include <CatalystEX/Loaders/Image.h>
#include <sstream>

static auto running = true ;

using API = cata::ivk::Vulkan ;

static void eventCallback( const cata::Event& event )
{
  if( event.type() == cata::Event::Type::WindowExit ) running = false ;
}

int main( int argc, const char* argv[] )
{
  if( argc != 2 ) { std::cout << "Usage: " << argv[ 0 ] << " <video_file>" << std::endl ; return 0 ; }
 
  auto cfg = omega::LoaderConfig() ;
  cfg.path = argv[ 1 ] ;
  omega::Loader loader( cfg ) ;
  
  API::addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
  API::addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
  API::initialize() ;
  
  auto tex_info  = cata::TextureInfo() ;
  
  tex_info.setFormat( cata::Format::RGBA8 ) ;
  tex_info.setWidth ( loader.width()      ) ;
  tex_info.setHeight( loader.height()     ) ;
  
  auto window    = cata::os::Window               ( argv[ 1 ], 1280, 1024                                         ) ;
  auto swapchain = cata::Swapchain<API>           ( 0, window.handle(), true                                      ) ; 
  auto copy_cmd  = cata::Commands<API>            ( 0, cata::SubmitType::Graphics                                 ) ;
  auto texture   = cata::Texture<API>             ( 0, tex_info                                                   ) ;
  auto staging   = cata::Array<API, unsigned char>( 0, loader.width() * loader.height() * loader.channels(), true ) ;
  
  window.setResizable( true ) ;

  cata::EventManager manager ;
  manager.enroll( &eventCallback, "Quit Callback" ) ;
  
  copy_cmd.copy( staging, texture                          ) ;
  copy_cmd.blit( texture, swapchain, cata::Filter::Nearest ) ;

  swapchain.wait( copy_cmd ) ;
  
  while( running )
  {
    loader.advance() ;
    auto frame = loader.frame() ;
    copy_cmd.copy( frame.pixels.data(), staging ) ;
    
    try 
    {
      swapchain.present() ;
    }
    catch( std::exception& e )
    {
      copy_cmd.copy( staging, texture                          ) ;
      copy_cmd.blit( texture, swapchain, cata::Filter::Nearest ) ;
    }

    window.poll() ;
  }
  
  copy_cmd.synchronize() ;
  return 0 ;
}