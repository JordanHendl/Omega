#include "Context.h"
#include <stdexcept>
#include <vector>

namespace omega
{
  auto throwError( const char* file, const char* description ) -> void 
  {
    throw std::runtime_error( description + std::string( "\n-- File: " ) + std::string( file ) ) ;
  }

  Context::Context( const char* path )
  {
    this->m_context = nullptr ;
    
//    av_log_set_level( AV_LOG_QUIET ) ;
    avformat_open_input( &this->m_context, path, nullptr, nullptr ) ;
    if( !this->m_context ) throwError( path, "Failed to open file." ) ;
    
    if( avformat_find_stream_info( this->m_context, nullptr ) < 0 ) throwError( path, "Failed to find any stream information." ) ;
    
    for( unsigned index = 0; index < this->m_context->nb_streams; index++ )
    {
      auto stream = this->m_context->streams[ index ] ;
      switch( stream->codecpar->codec_type )
      {
        case AVMediaType::AVMEDIA_TYPE_VIDEO : this->m_codecs.push_back( std::make_shared<VideoCodec>( stream, index ) ) ; break ;
        case AVMediaType::AVMEDIA_TYPE_AUDIO : this->m_codecs.push_back( std::make_shared<AudioCodec>( stream, index ) ) ; break ;
        default : break ;
      }
    }
  }
  
  Context::~Context()
  {
    avformat_close_input( &this->m_context ) ;
  }

  auto Context::context() -> AVFormatContext&
  {
    return *this->m_context ;
  }
  
  auto Context::codecs() -> std::vector<std::shared_ptr<Codec>>&
  {
    return this->m_codecs ;
  }
  
  auto Context::video() -> VideoCodec&
  {
    for( auto codec : this->codecs() ) if( codec->type() == AVMediaType::AVMEDIA_TYPE_VIDEO ) return *dynamic_cast<VideoCodec*>( codec.get() ) ;
    throw std::runtime_error( "No video codec found." ) ;
  }

  auto Context::audio() -> AudioCodec&
  {
    for( auto codec : this->codecs() ) if( codec->type() == AVMediaType::AVMEDIA_TYPE_AUDIO ) return *dynamic_cast<AudioCodec*>( codec.get() ) ;
    throw std::runtime_error( "No audio codec found." ) ;
  }
  
  auto DecodingContext::getFrame() -> void
  {
    this->m_running = true ;
    
    while( this->m_running )
    {
      while( !this->playing() ) { std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) ) ; }
      auto waiting = true ;
      while( waiting )
      {
        auto done = true ;
        for( auto& decoder : this->m_decoders )
        {
          if( !decoder->waiting() ) done = false ;
        }
        
        if( done ) waiting = false ;
        else std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) ) ;
      };
      
      if( av_read_frame( this->m_context, this->m_packet ) >= 0 )
      {
        for( auto& decoder : this->m_decoders )
        {
          decoder->decode( *this->m_packet ) ;
          
          if( !decoder->frames().empty() )
          {
            this->m_cb->pulse( decoder->codec().type(), *decoder->frames().top() ) ;
            decoder->frames().pop() ;
          }
        }
      }
      else
      {
        this->m_play = false ;
      }
    }
  }
}