#include "Callbacks.h"

namespace omega
{
  ConversionCallbacks::ConversionCallbacks( Callbacks& cbs )
  {
    this->recievers = &cbs ;
    
    this->audio_cb = nullptr ;
    this->video_cb = nullptr ;
  }
      
  ConversionCallbacks::~ConversionCallbacks()
  {
    if( this->audio_cb ) delete this->audio_cb ;
    if( this->video_cb ) delete this->video_cb ;
  }
  
  auto ConversionCallbacks::add( AudioCallback cb ) -> void
  {
    if( this->audio_cb ) delete this->audio_cb ;
    audio_cb = new FrameFunctionSubscriber<Audio>( cb ) ;
  }
  
  auto ConversionCallbacks::add( VideoCallback cb ) -> void
  {
    if( this->video_cb ) delete this->video_cb ;
    video_cb = new FrameFunctionSubscriber<Image>( cb ) ;
  }
  
  auto ConversionCallbacks::pulse( AVMediaType type, const Frame& frame ) -> void
  {
    switch( type )
    {
      case AVMediaType::AVMEDIA_TYPE_AUDIO : 
        if( this->recievers->audio_cb && this->audio_cb )
        {
          this->recievers->audio_cb->execute( this->audio_cb->execute( frame ) ) ;
        }
        break ;
      case AVMediaType::AVMEDIA_TYPE_VIDEO :
        if( this->recievers->video_cb && this->video_cb )
        {
          this->recievers->video_cb->execute( this->video_cb->execute( frame ) ) ;
        }
        break ;
      default : break ;
    }
  }
}