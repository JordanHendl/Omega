#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include <vector>
#include <memory>


namespace omega
{
  enum class Format : int ;
  class Frame ;
  
  auto channelsFromFormat( AVPixelFormat fmt ) -> int ;
  
  auto convert( AVPixelFormat fmt ) -> Format ;
  
  auto convert( Format fmt ) -> AVPixelFormat ;

  class Scale
  {
    public:
      Scale( AVPixelFormat src, AVPixelFormat dst ) ;
      
      ~Scale() ;
      
      auto convert( const AVFrame& src, AVFrame& dst ) -> void ;
      
    private:
      struct ScaleData ;
      std::unique_ptr<ScaleData> data ;
  };
}
