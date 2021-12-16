#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include "../Structures.h"
#include <vector>

namespace omega
{
  class Context ;
  
  auto validVideo( AVFrame& frame ) -> bool ;
  
  auto validAudio( AVFrame& frame ) -> bool ;
  
  class Decoder
  {
    public:
      Decoder( Context& context ) ;
      
      ~Decoder() ;
      
      auto advance() -> void ;
      
      auto seek( int time ) -> void ;
      
      auto current() -> int ;

      auto total() -> int ;
      
      auto video() -> AVFrame& ;
      
      auto audio() -> AVFrame& ;
      
    private:
      Context*              m_ctx    ;
      std::vector<AVFrame*> m_frames ;
      AVPacket*             m_packet ;
  };
}

