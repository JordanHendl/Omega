#include "Decoder.h"
#include "Context.h"

namespace omega
{
  constexpr auto NUM_FRAMES = 2 ;
  
  enum Frame
  {
    Video,
    Audio
  };
  
  auto validVideo( AVFrame& frame ) -> bool
  {
    return frame.width != 0 && frame.height != 0 ;
  }

  Decoder::Decoder( Context& context )
  {
    this->m_ctx = &context ;
    
    this->m_packet = av_packet_alloc() ;

    this->m_frames.resize( NUM_FRAMES ) ;
    for( auto& frame : this->m_frames )
    {
      frame = av_frame_alloc() ;
    }
  }
  
  Decoder::~Decoder()
  {
    this->m_ctx = nullptr ;
    
    for( auto& frame : this->m_frames )
    {
      av_frame_free( &frame ) ;
    }
    
    av_packet_free( &this->m_packet ) ;
  }
  
  auto Decoder::advance() -> void
  {
    if( av_read_frame( &this->m_ctx->context(), this->m_packet ) >= 0 )
    {
      avcodec_send_packet( &this->m_ctx->video().context(), this->m_packet ) ;
      
      if( this->m_packet->stream_index == this->m_ctx->video().index() )
      {
        avcodec_receive_frame( &this->m_ctx->video().context(), &this->video() ) ;
      }
      
      if( this->m_packet->stream_index == this->m_ctx->audio().index() )
      {
        //TODO implement audio
//        avcodec_receive_frame( &this->m_ctx->video().context(), &this->video() ) ;
      }
    }
  }
  
  auto Decoder::seek( int time ) -> void
  {
    
  }
  
  auto Decoder::video() -> AVFrame&
  {
    return *this->m_frames[ Frame::Video ] ;
  }
  
  auto Decoder::audio() -> AVFrame&
  {
    return *this->m_frames[ Frame::Audio ] ;
  }
}