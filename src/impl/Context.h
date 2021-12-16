#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include "Codec.h"
#include <memory>
#include <vector>

namespace omega
{
  class Context
  {
    public:
      Context( const char* path ) ;
      
      ~Context() ;

      auto context() -> AVFormatContext& ;
      
      auto codecs() -> std::vector<std::shared_ptr<Codec>>& ;
      
      auto video() -> VideoCodec& ;

      auto audio() -> AudioCodec& ;

    private:
      AVFormatContext*                    m_context ;
      std::vector<std::shared_ptr<Codec>> m_codecs  ;
  };
}