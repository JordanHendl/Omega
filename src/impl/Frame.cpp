#include "Frame.h"
#include <memory>

namespace omega
{
  Frame::Frame()
  {
    this->frame = nullptr ;
    this->frame = av_frame_alloc() ;
  }
  
  Frame::Frame( Frame&& mv )
  {
    this->frame = av_frame_alloc() ;
    *this = std::move( mv ) ;
  }

  Frame::~Frame()
  {
    if( this->frame )
    {
      av_frame_free( &this->frame ) ;
      this->frame = nullptr ;
    }
  }
  
  auto Frame::operator=( Frame&& mv ) -> Frame&
  {
    auto* tmp = this->frame ;
    this->frame = mv.frame ;
    mv.frame    = tmp      ;
    
    return *this ;
  }

  auto Frame::av() -> AVFrame&
  {
    return *this->frame ;
  }
  
  auto Frame::av() const -> const AVFrame&
  {
    return *this->frame ;
  }
  
  auto Frame::operator->() -> AVFrame*
  {
    return this->frame ;
  }
  
  auto Frame::operator->() const -> const AVFrame*
  {
    return this->frame ;
  }
  
  auto Frame::operator*() -> AVFrame&
  {
    return this->av() ;
  }
  
  auto Frame::operator*() const -> const AVFrame&
  {
    return this->av() ;
  }
  
  AudioFrame::~AudioFrame()
  {
    auto channels = this->channels() ;
    for( int index = 0; index < channels; index++ )
    {
      delete this->frame->data[ index ] ;
    }
  }
}