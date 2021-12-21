#include <Omega.h>
#include <iostream>
#include <Catalyst/vk/Vulkan.h>
#include <Catalyst/window/Window.h>
#include <Catalyst/window/Event.h>
#include <Catalyst/structures/Structures.h>
#include <CatalystEX/Loaders/Image.h>
#include <Hathor/Hathor.h>
#include <Hathor/Stream.h>
#include <Hathor/Structures.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <memory>

static auto running = true ;

using API = cata::ivk::Vulkan ;

static hathor::Stream* stream ;

static omega::Container container ;

static void eventCallback( const cata::Event& event )
{
  if( event.type() == cata::Event::Type::WindowExit ) running = false ;
}

static auto audioCallback( const omega::Audio& sound )  -> void 
{
    auto info = hathor::PlayInfo() ;
    stream->play( info, sound.data.data(), sound.data.size() * sizeof( unsigned short ), sound.sample_rate, hathor::Format::Stereo16 ) ;
}

struct WindowApp
{
  cata::Commands<API>             copy_cmd  ;
  cata::Texture<API>              texture   ;
  cata::Array<API, unsigned char> staging   ;
  
  auto videoCallback( const omega::Image& img ) -> void
  {
    this->copy_cmd.copy( img.pixels.data(), this->staging ) ;
  }
};

int main( int argc, const char* argv[] )
{
  if( argc != 2 ) { std::cout << "Usage: " << argv[ 0 ] << " <video_file>" << std::endl ; return 0 ; }
 
  auto timer = omega::Timer       () ;
  auto app   = WindowApp          () ;
  auto cfg   = omega::LoaderConfig() ;
  auto cbs   = omega::Callbacks   () ;
  
  cfg.path         = argv[ 1 ] ;
  cfg.enable_audio = true      ;
  
  cbs.add( &audioCallback                  ) ;
  cbs.add( &app, &WindowApp::videoCallback ) ;
  omega::Loader loader( cfg, cbs ) ;
  
  API::addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
  API::addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
  API::initialize() ;
  
  auto tex_info  = cata::TextureInfo() ;
  
  tex_info.setFormat( cata::Format::RGBA8 ) ;
  tex_info.setWidth ( loader.width()      ) ;
  tex_info.setHeight( loader.height()     ) ;
  
  auto window    = cata::os::Window               ( argv[ 1 ], 1920, 1024                                           ) ;
  auto swapchain = cata::Swapchain<API>           ( 0, window.handle(), true                                        ) ; 
  auto copy_cmd  = cata::Commands<API>            ( 0, cata::SubmitType::Graphics                                   ) ;
  auto texture   = cata::Texture<API>             ( 0, tex_info                                                     ) ;
  auto staging   = cata::Array<API, unsigned char>( 0, loader.width() * loader.height() * loader.channels(), true   ) ;
  app.copy_cmd.swap( copy_cmd ) ;
  app.texture .swap( texture  ) ;
  app.staging .swap( staging  ) ;

  window.setResizable( true ) ;

  cata::EventManager manager ;
  manager.enroll( &eventCallback, "Quit Callback" ) ;
  
  app.copy_cmd.copy( app.staging, app.texture                      ) ;
  app.copy_cmd.blit( app.texture, swapchain, cata::Filter::Nearest ) ;

  swapchain.wait( app.copy_cmd ) ;
  
  std::cout << loader.info() << std::endl ;
  
  hathor::Library::initialize() ;
  stream = new hathor::Stream() ;
  
  loader.play() ;
  timer.start() ;
  while( running )
  { 
    try 
    {
      swapchain.present() ;
    }
    catch( std::exception& e )
    {
      app.copy_cmd.copy( app.staging, app.texture                      ) ;
      app.copy_cmd.blit( app.texture, swapchain, cata::Filter::Nearest ) ;
    }
    window.poll() ;
  }
  
  app.copy_cmd.synchronize() ;
  return 0 ;
}