/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Conversion.cpp
 * Author: jhendl
 * 
 * Created on December 15, 2021, 1:39 PM
 */

#include "Conversion.h"
#include "Structures.h"
#include <Omega.h>

// Make sure to do this for all ffmpepg includes. C headers in C++ application uhhh apparently don't mix.
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
  #include <libswresample/swresample.h>
  #include <libavutil/samplefmt.h>
  #include <libavutil/opt.h>
  #include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
}

namespace omega
{
  struct Scale::ScaleData
  {
    SwsContext* ctx ;

    ScaleData()
    {
      this->ctx = nullptr ;
    }
    
    ~ScaleData()
    {
      
    }
    
    auto initialize( const Frame& src, Frame& dst ) -> void 
    {
      if( this->ctx ) { sws_freeContext( this->ctx ) ; this->ctx = nullptr ; }
      
      auto src_format = static_cast<AVPixelFormat>( src->format ) ;
      auto dst_format = static_cast<AVPixelFormat>( dst->format ) ;
      
      this->ctx = sws_getContext( src->width, src->height, src_format, dst->width, dst->height, dst_format, SWS_BICUBIC, nullptr, nullptr, nullptr ) ;
    }
  };
  
  struct Resample::ResampleData
  { 
    SwrContext* ctx ;
    
    ResampleData()
    {
      this->ctx = nullptr ;
    }
    
    ~ResampleData()
    {
      
    }
    
    auto reinitialize( const Frame& src, const Frame& dst ) -> void 
    {
      if( this->ctx ) { swr_free( &this->ctx ) ; this->ctx = nullptr ; }
      this->ctx = swr_alloc() ;

      if( !this->ctx                                              ) throw std::runtime_error( "Failed to allocate resample context." ) ;
      if( swr_config_frame( this->ctx, &dst.av(), &src.av() ) < 0 ) throw std::runtime_error( "Failed to configure swresample."      ) ;
      if( ( swr_init( this->ctx ) )                           < 0 ) throw std::runtime_error( "Failed to initialize swresample."     ) ;
    }
  };

  Scale::Scale()
  {
    this->data = std::make_unique<ScaleData>() ;
  }
  
  Scale::~Scale()
  {
    
  }
  
  auto Scale::convert( const Frame& src, Frame& dst ) -> void 
  {
    if( !data->ctx ) data->initialize( src, dst ) ;
    if( sws_scale( data->ctx, src->data, src->linesize, 0, src->height, dst->data, dst->linesize ) < 0 ) throw std::runtime_error( "Failed to resample image." ) ;
  }

  Resample::Resample()
  {
    this->data = std::make_unique<ResampleData>() ;
  }
  
  Resample::~Resample()
  {
    
  }
  
  auto Resample::convert( const Frame& src, Frame& dst ) -> void 
  {
    if( data->ctx == nullptr )
    {
      data->reinitialize( src, dst ) ;
    }
    
    auto& ctx = data->ctx ;
    
    dst.av().nb_samples = 0 ;
    auto result = swr_convert_frame( ctx, &dst.av(), &src.av() ) ;
    if( result < 0 ) throw std::runtime_error( "Failed to resample frame." ) ;
  }
}
