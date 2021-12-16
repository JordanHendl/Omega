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
    
    this->m_params = codec_params             ;
    this->m_codec  = codec                    ;
    this->m_type   = codec_params->codec_type ;
    
    auto context = avcodec_alloc_context3( codec ) ;
    
    if( !context ) throw std::runtime_error( "Failed to create codec context." ) ;
    avcodec_parameters_to_context( context, codec_params ) ;
    avcodec_open2( context, codec, nullptr ) ;
    
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
}
