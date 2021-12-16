#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

namespace omega
{
  class Codec
  {
    public:
      Codec( AVStream* stream, int index ) ;
      
      virtual ~Codec() ;
      
      virtual auto type() -> AVMediaType ;
      
      virtual auto codec() -> AVCodec& ;
      
      virtual auto params() -> AVCodecParameters& ;
      
      virtual auto index() -> int ;
      
      virtual auto context() -> AVCodecContext& ;
    protected:
      AVCodecParameters* m_params ;
      AVCodec*           m_codec  ;
      AVCodecContext*    m_ctx    ;
      AVMediaType        m_type   ;
      int                m_index  ;
  };
  
  class VideoCodec : public Codec
  {
    public :
      
      VideoCodec( AVStream* stream, int index ) : Codec( stream, index ) {} ;
      
      auto width() -> int ;
      
      auto height() -> int ;
      
      auto channels() -> int ;
      
      auto format() -> AVPixelFormat ;
  };
  
  class AudioCodec : public Codec
  {
    public :
      
      AudioCodec( AVStream* stream, int index ) : Codec( stream, index ) {} ;
  };
}
