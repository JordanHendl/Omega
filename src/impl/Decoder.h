#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include <Structures.h>
#include "Frame.h"
#include "Callbacks.h"
#include <condition_variable>
#include <vector>
#include <stack>
#include <memory>
#include <mutex>

namespace omega
{
  class Context ;
  class Codec   ;

  auto validVideo( AVFrame& frame ) -> bool ;
  
  auto validAudio( AVFrame& frame ) -> bool ;
  
  class Decoder
  {
    public:
      Decoder( Codec& codec ) ;
      
      ~Decoder() ;

      auto loop( ConversionCallbacks& cb, AVPacket& packet ) -> void ;
      
      auto signal() -> void ;

      auto waiting() -> bool ;
      
      auto decode( AVPacket& packet ) -> bool ;
      
      auto frames() -> std::stack<std::shared_ptr<Frame>>& ;
      
      auto codec() -> Codec& ;
    private:
      
      Codec*                             m_codec    ;
      AVPacket*                          m_packet   ;
      std::stack<std::shared_ptr<Frame>> m_frames   ;
      bool                               m_finished ;
      bool                               m_ready    ;
      
      std::mutex              mutex ;
      std::condition_variable cv    ;
  };
}