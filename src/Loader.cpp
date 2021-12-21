#include "Loader.h"
#include "Timer.h"
#include "impl/Context.h"
#include "impl/Decoder.h"
#include "impl/Conversion.h"
#include "impl/Frame.h"
#include "impl/Callbacks.h"
#include <sstream>

#include <iostream>
#include <libavutil/mathematics.h>

namespace omega
{
  struct Loader::LoaderData
  {
    std::unique_ptr<ConversionCallbacks> conversion   ;
    Callbacks*                           cb           ;
    std::unique_ptr<DecodingContext>     context      ;
    std::unique_ptr<Scale          >     scale        ;
    AudioFrame                           audio_output ;
    Resample                             resample     ;
    LoaderConfig                         cfg          ;
    Frame                                frame        ;
    Image                                output_img   ;
    Audio                                output_wav   ;
    Timer                                timer        ;
    int                                  video_frame_count  ;
    int                                  audio_frame_count  ;

    auto convertAudio( const Frame& frame ) -> const Audio&
    {
      if( this->cfg.enable_audio )
      {
        auto ptr  = reinterpret_cast<unsigned short*>( this->audio_output->data[ 0 ] ) ;
        auto size = frame->nb_samples * 2 ;
        // Convert frame to signed 16 bit interleaved pcm.
        this->resample.convert( frame, this->audio_output ) ;
        this->output_wav.sample_rate = frame->sample_rate ;
        this->output_wav.data.assign( ptr, ptr + size ) ; 
        this->output_wav.timestamp   = ( frame.av().pts * this->context->audio().timeBase() ) * 1000.f ;
      }

      if( this->cfg.wait_for_next_frame )
      {
        while( this->timer.time() < this->output_wav.timestamp ) {} ;
      }

      return this->output_wav ;
    }
    
    auto convertVideo( const Frame& frame ) -> const Image&
    {
      // Convert frame to RGBA8.
      this->scale->convert( frame, this->frame ) ;
      this->output_img.timestamp = ( frame.av().pts * this->context->video().timeBase() ) * 1000.f ;
      
      if( this->cfg.wait_for_next_frame )
      {
        while( this->timer.time() < this->output_img.timestamp ) {} ;
      }
      
      return this->output_img ;
    }
    
    LoaderData( const LoaderConfig& cfg, Callbacks& cb )
    {
      // TODO handle other formats.
      constexpr auto out_format = AVPixelFormat::AV_PIX_FMT_RGBA ;
      
      this->cfg = cfg ;
      this->cb  = &cb ;

      this->conversion = std::make_unique<ConversionCallbacks>( cb ) ;
      this->conversion->add( this, &LoaderData::convertAudio ) ;
      this->conversion->add( this, &LoaderData::convertVideo ) ;
      
      this->context    = std::make_unique<DecodingContext    >( cfg.path.c_str(), *this->conversion ) ;
      this->scale      = std::make_unique<Scale              >(                                     ) ;

      this->audio_output = std::move( AudioFrame( AVSampleFormat::AV_SAMPLE_FMT_S16, AV_CH_LAYOUT_STEREO, this->context->audio().sampleRate() ) ) ;
      
      auto& video = this->context->video() ;
      this->output_img.width    = video.width () ;
      this->output_img.height   = video.height() ;
      this->output_img.channels = 4              ;
      
      const auto size = this->output_img.width * this->output_img.height * this->output_img.channels ;
      this->output_img.pixels.resize( size ) ;
      
      this->frame->width  = video.width   () ;
      this->frame->height = video.height  () ;
      this->frame->format = out_format       ;
      
      this->frame->linesize[ 0 ] = video.width() * this->output_img.channels ;
      this->frame->data    [ 0 ] = this->output_img.pixels.data() ;
      
      this->video_frame_count = 0 ;
      this->audio_frame_count = 0 ;
    }
    
    auto infoString() -> std::string
    {
      std::stringstream str ;
      
      str << "Omega Video Loader Info\n" ;
      str << "------------------------------------------------\n" ;
      str << "Format: " << this->context->context().iformat->long_name << ", Duration: " <<  this->context->context().duration << "\n" ;
      str << "FPS: " << this->context->video().fps() << "\n" ;

      int index = 0 ;
      for( auto& codec : this->context->codecs() )
      {
        str << "Codec#: " << index << "\n" ;
        str << "Name: " << codec->codec().long_name << " id: " << codec->codec().id << " bit_rate: " << codec->params().bit_rate << "\n" ;
        switch( codec->params().codec_type )
        {
          case AVMEDIA_TYPE_VIDEO :
            str << "-- Resolution: " << codec->params().width << " " << codec->params().height << "\n" ;
            break ;
          case AVMEDIA_TYPE_AUDIO :
            str << "--Audio Info: Channels-> " << codec->params().channels << " | Sample Rate-> " << codec->params().sample_rate << "\n" ;
            break ;
          default : break ;
        }
        
        str << "\n" ;
      }
      
      return str.str() ;
    }
  };
  
  Loader::Loader( const LoaderConfig& cfg, Callbacks& cb )
  {
    this->data = std::make_unique<LoaderData>( cfg, cb ) ;
  }
  
  Loader::~Loader()
  {
    data->context->stop() ;
  }
  
  auto Loader::info() -> std::string
  {
    return data->infoString() ;
  }
  
  auto Loader::play() -> void
  {
    if( !data->timer.initialized() ) 
    {
      data->timer.initialize( "" ) ;
      data->timer.start() ;
    }
    
    data->context->play() ;
  }
  
  auto Loader::pause() -> void
  {
    data->context->pause() ;
  }
  
  auto Loader::advance() -> void
  {

    
//    data->decoder->advance() ;
//    
//    if( !data->decoder->video().empty() )
//    {
//      auto decoded_video_ptr = data->decoder->video().top() ;
//      auto& decoded_video = *decoded_video_ptr ;
//      
//      data->decoder->video().pop() ;
//      data->decoder->audio().pop() ;
//  
//      data->scale->convert( decoded_video, data->frame ) ;
//      data->output_img.timestamp = ( data->frame_count / data->context->video().fps() ) * 1000.f ;
//    }
//    
//    if( data->cfg.enable_audio && !data->decoder->audio().empty() )
//    {
//      auto decoded_audio_ptr = data->decoder->audio().top() ;
//      auto& decoded_audio = *decoded_audio_ptr ;
//      auto& out_audio = data->audio_output ;
//      
//      
//      data->resample.convert( decoded_audio, out_audio ) ;
//
//      data->output_wav.timestamp   = ( data->frame_count / data->context->audio().fps() ) * 1000.f ;
//      data->output_wav.sample_rate = out_audio->sample_rate ;
//      data->output_wav.count       = out_audio->nb_samples * 2 ;
//      data->output_wav.data        = reinterpret_cast<unsigned short*>( out_audio->data[ 0 ] ); 
//    }
//    if( data->cfg.wait_for_next_frame )
//    {
//      while( data->timer.time() + 100.f < data->output_img.timestamp ) {} ;
//      std::cout << "Loader time: " << data->timer.time() << std::endl ;
//    }
//    
//    data->frame_count++ ;
  }
  
  auto Loader::playing() -> bool
  {
    return data->context->playing() ;
  }
  
  auto Loader::seek( int index ) -> void
  {
    data->context->seek( index ) ;
  }
  
  auto Loader::duration() -> float
  {
    return data->context->video().duration() ;
  }
  
  auto Loader::width() -> int
  {
    return data->context->video().width() ;
  }
  
  auto Loader::height() -> int
  {
    return data->context->video().height() ;
  }
  
  auto Loader::channels() -> int
  {
    return 4 ;
  }
  
  auto Loader::frequency() -> int
  {
    return data->context->audio().sampleRate() ;
  }
}