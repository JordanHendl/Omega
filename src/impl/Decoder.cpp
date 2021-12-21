#include "Decoder.h"
#include "Codec.h"
#include <iostream>
#include <string>
#include <libavcodec/avcodec.h>

namespace omega
{
  constexpr auto NUM_FRAMES = 2 ;
  
  enum FrameID
  {
    Video = 0,
    Audio = 1
  };
  
  auto validVideo( AVFrame& frame ) -> bool
  {
    return frame.width != 0 && frame.height != 0 ;
  }
  
  auto validAudio( AVFrame& frame ) -> bool
  {
    return frame.sample_rate != 0 && frame.channels != 0 ;
  }

  auto Decoder::loop( ConversionCallbacks& cb, AVPacket& ) -> void
  {
    while( !this->m_finished )
    {
      std::unique_lock<std::mutex> lock( this->mutex ) ;
      this->cv.wait( lock, [=] { return this->m_ready ; } ) ;
      
      if( this->decode( *this->m_packet ) && !this->m_frames.empty() )
      {
        cb.pulse( this->m_codec->type(), *this->m_frames.top() ) ;
        this->m_frames.pop() ;
      }
      
      this->m_ready = false ;
    }
  }
  
  auto Decoder::signal() -> void
  {
    {
      std::scoped_lock<std::mutex> lock( this->mutex ) ;
      this->m_ready = true ;
    }
    this->cv.notify_all() ;
  }

  auto Decoder::waiting() -> bool
  {
    return !this->m_ready ;
  }

  auto Decoder::decode( AVPacket& packet ) -> bool
  {
    auto complete = false ;
    
    static auto frame = std::make_shared<Frame>() ;
    
    if( packet.stream_index == this->m_codec->index() )
    {
      avcodec_send_packet( &this->m_codec->context(), &packet ) ;
      if( avcodec_receive_frame( &this->m_codec->context(), &frame->av() ) >= 0 )
      {
        this->m_frames.push( frame ) ;
        complete = true ;
      }
      av_packet_unref( &packet ) ;
    }
    else
    {
      complete = true ;
    }

    return complete ;
  }
  
  auto Decoder::codec() -> Codec&
  {
    return *this->m_codec ;
  }

  auto Decoder::frames() -> std::stack<std::shared_ptr<Frame>>&
  {
    return this->m_frames ;
  }

  Decoder::Decoder( Codec& codec )
  {
    this->m_codec    = &codec ;
    this->m_finished = false  ;
    this->m_ready    = false  ;
    this->m_packet = av_packet_alloc() ;
  }
  
  Decoder::~Decoder()
  {
    this->m_codec = nullptr ;
    av_packet_free( &this->m_packet ) ;
  }
  
//  auto Decoder::advance() -> void
//  {
//    while( ( this->audio().empty() || this->video().empty() ) && this->m_finished == false )
//    {
//      auto tmp = std::make_shared<Frame>() ;
//      if( av_read_frame( &this->m_ctx->context(), this->m_packet ) >= 0 )
//      {
//             if( decode( this->m_ctx->video(), *this->m_packet, *tmp ) ) this->video().push( tmp ) ;
//        else if( decode( this->m_ctx->audio(), *this->m_packet, *tmp ) ) this->audio().push( tmp ) ;
//      }
//      else
//      {
//        this->m_finished = true ;
//      }
//    }
//  }
//  
//  auto Decoder::seek( int ) -> void
//  {
////    audio().
//  }
//  
//  auto Decoder::video() -> std::stack<std::shared_ptr<Frame>>&
//  {
//    return this->m_frames[ FrameID::Video ] ;
//  }
//  
//  auto Decoder::audio() -> std::stack<std::shared_ptr<Frame>>&
//  {
//    return this->m_frames[ FrameID::Audio ] ;
//  }
//  
//  auto Decoder::empty() -> bool
//  {
//    return this->m_finished ;
//  }
}