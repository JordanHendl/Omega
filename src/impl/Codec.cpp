/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Codec.cpp
 * Author: jhendl
 * 
 * Created on December 16, 2021, 9:05 AM
 */

#include "Codec.h"
#include <stdexcept>

namespace omega
{
  Codec::Codec( AVStream* stream, int index )
  {
    auto codec_params = stream->codecpar ;
    auto codec        = avcodec_find_decoder( codec_params->codec_id ) ;
    
    this->m_stream = stream                   ;
    this->m_params = codec_params             ;
    this->m_codec  = codec                    ;
    this->m_type   = codec_params->codec_type ;
    auto context = avcodec_alloc_context3( codec ) ;
    
    if( !context ) throw std::runtime_error( "Failed to create codec context." ) ;

    if( avcodec_parameters_to_context( context, codec_params ) < 0 ) throw std::runtime_error( "Failed to bind codec parameters to the context." ) ;
    
    context->thread_count = 0 ;
    
    if( codec->capabilities | AV_CODEC_CAP_FRAME_THREADS      ) context->thread_type = FF_THREAD_FRAME ;
    else if( codec->capabilities | AV_CODEC_CAP_SLICE_THREADS ) context->thread_count = FF_THREAD_SLICE ;
    else context->thread_count = 1 ;
    
    if( avcodec_open2( context, codec, nullptr ) < 0 ) throw std::runtime_error( "Failed to open codec." ) ;
    
    this->m_index = index   ;
    this->m_ctx   = context ;
  }
  
  Codec::~Codec()
  {
    
  }
  
  auto Codec::type() -> AVMediaType
  {
    return this->m_type ;
  }
  
  auto Codec::codec() -> AVCodec&
  {
    return *this->m_codec ;
  }
  
  auto Codec::index() -> int
  {
    return this->m_index ;
  }

  auto Codec::params() -> AVCodecParameters&
  {
    return *this->m_params ;
  }
  
  auto Codec::context() -> AVCodecContext&
  {
    return *this->m_ctx ;
  }
  
  auto Codec::duration() -> float
  {
    return static_cast<float>( this->m_stream->duration ) ;
  }
  
  auto Codec::fps() -> float
  {
    return static_cast<float>( this->m_stream->avg_frame_rate.num ) / static_cast<float>( this->m_stream->avg_frame_rate.den ) ;
  }
  
  auto Codec::stream() -> AVStream&
  {
    return *this->m_stream ;
  }

  auto Codec::timeBase() -> float
  {
    return static_cast<float>( this->m_stream->time_base.num ) / static_cast<float>( this->m_stream->time_base.den ) ;
  }

  auto VideoCodec::width() -> int
  {
    return this->m_params->width ;
  }
  
  auto VideoCodec::height() -> int
  {
    return this->m_params->height ;
  }
  
  auto VideoCodec::channels() -> int
  {
    switch( this->params().format )
    {
      case AV_PIX_FMT_YUV420P : return 1 ;
      default : return 3 ;
    }
  }
  
  auto VideoCodec::format() -> AVPixelFormat
  {
    return static_cast<AVPixelFormat>( this->params().format ) ;
  }
  
  auto AudioCodec::format() -> AVSampleFormat
  {
    return static_cast<AVSampleFormat>( this->params().format ) ;
  }
  
  auto AudioCodec::channels() -> int
  {
    return this->params().channels ;
  }
  
  auto AudioCodec::sampleRate() -> int
  {
    return this->params().sample_rate ;
  }
}
