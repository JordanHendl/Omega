#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include "Codec.h"
#include "Decoder.h"
#include "Callbacks.h"
#include <Structures.h>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <Structures.h>

namespace omega
{
  class Decoder ;
  class Encoder ;
  
  class Context
  {
    public:
      Context( const char* path ) ;
      
      virtual ~Context() ;

      virtual auto context() -> AVFormatContext& ;
      
      virtual auto codecs() -> std::vector<std::shared_ptr<Codec>>& ;
      
      virtual auto video() -> VideoCodec& ;

      virtual auto audio() -> AudioCodec& ;

    protected:
      AVFormatContext*                    m_context ;
      std::vector<std::shared_ptr<Codec>> m_codecs  ;
  };
  
  class DecodingContext : public Context
  {
    public:
      DecodingContext( const char* path, ConversionCallbacks& cb ) : Context( path )
      {
        this->m_cb   = &cb   ;
        this->m_play = false ;

        this->m_packet = av_packet_alloc() ;
        this->m_threads .resize ( this->m_codecs.size() + 1 ) ; 
        this->m_mutexes .resize ( this->m_codecs.size() + 1 ) ;
        this->m_decoders.resize ( this->m_codecs.size()     ) ;
        
        int index = 0 ;
        for( auto& mutex   : this->m_mutexes  ) mutex   = std::make_unique<std::mutex>(                            ) ;
        for( auto& decoder : this->m_decoders ) decoder = std::make_unique<Decoder   >( *this->m_codecs[ index++ ] ) ;
        
        //TODO make threads here, so that processing of decoded output is asynchronous.
        // ALSO make this an intermediary step to the final output. Im thinking the same callback structure, but to take in the Frame's and output Audio/Video objects.
        // cont. -> this allows for user defined conversions because most apps need to know what their output format is in.
        
        this->m_threads[ 0 ] = std::thread( &DecodingContext::getFrame, this ) ;
        
        this->m_running = true ;
      }
      
      auto play() -> void
      {
        this->m_play = true ;
        
      }
      
      auto pause() -> void
      {
        this->m_play = false ;
      }
      
      auto stop() -> void
      {
        if( this->m_running )
        {
          this->m_running = false ;
          this->m_play    = false ;
          this->m_threads[ 0 ].join() ;
        }
      }

      auto seek( int time ) -> void
      {
        auto tmp = this->m_play ;
        this->pause() ;
        for( auto& codec : this->codecs() )
        {
          av_seek_frame( this->m_context, codec->index(), static_cast<int64_t>( time ), 0 ) ;
        }
        
        this->m_play = tmp ;
      }
      
      auto playing() -> bool
      {
        return this->m_play ;
      }

      virtual ~DecodingContext()
      {
        if( this->m_running )
        {
          this->m_threads[ 0 ].join() ;
          this->m_running = false ;
          this->m_play    = false ;
        }
      } ;

    private:
      
      /** Method to retrieve the next frame and signal the other threads to process.
       */
      auto getFrame() -> void ;
      
      /** Callback object to respond to decoder outputs.
       */
      ConversionCallbacks* m_cb ;
      
      /** One thread for processing decoders, and one thread per handling decoder output.
       */
      std::vector<std::thread> m_threads ;
      
      /** The decoders.
       */
      std::vector<std::unique_ptr<Decoder>> m_decoders ;
      
      /** Mutexes to manage threads.
       */
      std::vector<std::unique_ptr<std::mutex>> m_mutexes  ;
      
      AVPacket* m_packet ;
      
      
      bool m_running ;
      bool m_play    ;
  };
}