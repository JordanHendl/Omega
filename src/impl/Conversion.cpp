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
}

namespace omega
{
  struct Scale::ScaleData
  {
    AVPixelFormat src_format ;
    AVPixelFormat dst_format ;
    
    SwsContext* ctx ;
    
    
    int src_width  ;
    int src_height ;
    
    int dst_width  ;
    int dst_height ;
    
    ScaleData()
    {
      this->ctx = nullptr ;
    }
    
    ~ScaleData()
    {
      
    }
    
    auto initialize( int sw, int sh, int dw, int dh ) -> void 
    {
      if( this->ctx ) { sws_freeContext( this->ctx ) ; this->ctx = nullptr ; }
      this->ctx = sws_getContext( sw, sh, this->src_format, dw, dh, this->dst_format, SWS_BICUBIC, nullptr, nullptr, nullptr ) ;
      
      this->src_width  = sw ;
      this->src_height = sh ;
      this->dst_width  = dw ;
      this->dst_height = dh ;
    }
    
    auto dimsMatch( const AVFrame& src, const AVFrame& dst ) -> bool 
    {
      return ( src.width == this->src_width && src.height == this->src_height ) &&
             ( dst.width == this->dst_width && dst.height == this->dst_height ) ;  
    }
  };

  Scale::Scale( AVPixelFormat src, AVPixelFormat dst )
  {
    this->data = std::make_unique<ScaleData>() ;
    
    this->data->src_format = src ;
    this->data->dst_format = dst ;
  }
  
  Scale::~Scale()
  {
    
  }
  
  auto Scale::convert( const AVFrame& src, AVFrame& dst ) -> void 
  {
    if( src.width != 0 && src.height != 0 ) 
    {
      if( !data->ctx || !data->dimsMatch( src, dst ) )
      {
        data->initialize( src.width, src.height, dst.width, dst.height ) ;
      }
      
      sws_scale( data->ctx, src.data, src.linesize, 0, src.height, dst.data, dst.linesize ) ;
    }
  }
}
