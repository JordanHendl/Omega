#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include <vector>
#include <memory>
#include "Frame.h"

namespace omega
{
  enum class Format : int ;
  class Frame ;
  
  auto channelsFromFormat( AVPixelFormat fmt ) -> int ;
  
  auto convert( AVPixelFormat fmt ) -> Format ;
  
  auto convert( Format fmt ) -> AVPixelFormat ;
  
  struct ResampleConfig
  {
    AVSampleFormat src ;
    AVSampleFormat dst ;
    
    ResampleConfig()
    {
      src = AVSampleFormat::AV_SAMPLE_FMT_S16 ;
      dst = AVSampleFormat::AV_SAMPLE_FMT_S16 ;
    }
  };
  
  class Scale
  {
    public:
      Scale() ;
      
      ~Scale() ;
      
      auto convert( const Frame& src, Frame& dst ) -> void ;
      
    private:
      struct ScaleData ;
      std::unique_ptr<ScaleData> data ;
  };
  
  class Resample
  {
    public:
      Resample() ;
      
      ~Resample() ;
      
      auto convert( const Frame& src, Frame& dst ) -> void ;
      
    private:
      struct ResampleData ;
      std::unique_ptr<ResampleData> data ;
  };
}
