#include "Loader.h"
#include "impl/Context.h"
#include "impl/Decoder.h"
#include "impl/Conversion.h"
namespace omega
{
  struct Loader::LoaderData
  {
    std::unique_ptr<Context> context ;
    std::unique_ptr<Decoder> decoder ;
    std::unique_ptr<Scale  > scale   ;
    LoaderConfig             cfg     ;
    AVFrame*                 frame   ;
    Image                    output  ;

    LoaderData( const LoaderConfig& cfg )
    {
      // TODO handle other formats.
      constexpr auto out_format = AVPixelFormat::AV_PIX_FMT_RGBA ;
      
      this->cfg = cfg ;
       
      this->context = std::make_unique<Context>( cfg.path.c_str() ) ;
      this->decoder = std::make_unique<Decoder>( *this->context   ) ;
      this->scale   = std::make_unique<Scale  >( this->context->video().format(), out_format ) ;
      
      auto& video = this->context->video() ;
      
      this->frame = av_frame_alloc() ;
      
      this->output.width    = video.width   () ;
      this->output.height   = video.height  () ;
      this->output.channels = 4                ;
      
      const auto size = this->output.width * this->output.height * this->output.channels ;
      this->output.pixels.resize( size ) ;

      this->frame->width    = video.width   () ;
      this->frame->height   = video.height  () ;
      this->frame->format   = out_format       ;
      
      this->frame->linesize[ 0 ] = video.width() * this->output.channels ;
      this->frame->data    [ 0 ] = this->output.pixels.data() ;
    }
  };
  
  Loader::Loader( const LoaderConfig& cfg )
  {
    this->data = std::make_unique<LoaderData>( cfg ) ;
  }
  
  Loader::~Loader()
  {
    
  }
  
  auto Loader::info() -> std::string
  {
//    return data->infoString() ;
  }
  
  auto Loader::frame() -> const Image&
  {
    return data->output ;
  }
  
  auto Loader::advance() -> void
  {
    if( !validVideo( data->decoder->video() ) )
    {
      while( !validVideo( data->decoder->video() ) )
      {
        data->decoder->advance() ;
      }
    }
    else
    {
      data->decoder->advance() ;
    }
    
    data->scale->convert( data->decoder->video(), *data->frame ) ;
  }
  
  auto Loader::seek( int index ) -> void
  {
    data->decoder->seek( index ) ;
  }
  
  auto Loader::count() -> int
  {
//    return data->format_context->duration ;
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
}


























//
//// Make sure to do this for all ffmpepg includes. C headers in C++ application uhhh apparently don't mix.
//extern "C"
//{
//  #include <libavcodec/avcodec.h>
//  #include <libavformat/avformat.h>
//  #include <libswscale/swscale.h>
//}
//
//#include <iostream>
//#include <stdexcept>
//#include <vector>
//#include <string>
//#include <sstream>
//#include <impl/Conversion.h>
//
//namespace omega
//{
//  struct DecodedFrame
//  {
//    AVFrame*  frame  ;
//    AVPacket* packet ;
//  };
//
//  struct CodecData
//  {
//    AVCodecParameters* params ;
//    AVCodec*           codec  ;
//    AVCodecContext*    ctx    ;
//    AVMediaType        type   ;
//    
//    CodecData( AVStream* stream )
//    {
//      auto codec_params = stream->codecpar ;
//      auto codec        = avcodec_find_decoder( codec_params->codec_id ) ;
//      
//      this->params = codec_params             ;
//      this->codec  = codec                    ;
//      this->type   = codec_params->codec_type ;
//      
//      auto context = avcodec_alloc_context3( codec ) ;
//      avcodec_parameters_to_context( context, codec_params ) ;
//      avcodec_open2( context, codec, nullptr ) ;
//      
//      this->ctx = context ;
//    }
//    
//    auto width() -> int
//    {
//      return this->params->width ;
//    }
//    
//    auto height() -> int
//    {
//      return this->params->height ;
//    }
//    
//    auto channels() -> int
//    {
//      if( this->params->format == AV_PIX_FMT_YUV420P ) return 1 ;
//      return 3 ;
//    }
//    
//    auto is() -> bool
//    {
//      return this->type == AVMEDIA_TYPE_VIDEO ;
//    }
//    
//    auto isAudio() -> bool
//    {
//      return this->type == AVMEDIA_TYPE_AUDIO ;
//    }
//  };
//  
//
//  struct Loader::LoaderData
//  {
//    AVFrame*               av_frame       ;
//    AVFrame*               av_frame_output;
//    AVPacket*              av_packet      ;
//    Frame                  frame          ;
//    AVFormatContext*       format_context ;
//    std::vector<CodecData> codecs         ;
//    std::string            path           ;
//    int                    pos            ;
//    int                    audio_index    ;
//    int                    video_index    ;
//    int                    width          ;
//    int                    height         ;
//    int                    channels       ;
//
//    auto throwError( std::string error_str ) -> void
//    {
//      throw std::runtime_error( error_str + std::string( "\n | File: " ) + this->path ) ;
//    }
//    
//    auto decode( AVPacket* packet, CodecData& codec, AVFrame* frame ) -> void
//    {
//      avcodec_send_packet  ( codec.ctx, packet ) ;
//      avcodec_receive_frame( codec.ctx, frame  ) ;
//    }
//    
//    auto decodeAudio( AVPacket* packet, CodecData& codec, AVFrame* frame ) -> void
//    {
//      avcodec_send_packet  ( codec.ctx, packet ) ;
//      avcodec_receive_frame( codec.ctx, frame  ) ;
//    }
//    
//    LoaderData( const char* path )
//    {
//      this->av_frame_output = av_frame_alloc() ;
//      this->format_context = nullptr ;
//      this->path           = path    ;
//      this->pos            = 0       ;
//      this->video_index    = -1      ;
//      this->audio_index    = -1      ;
//      this->av_frame       = nullptr ;
//      this->av_packet      = nullptr ; 
//
//      avformat_open_input( &this->format_context, path, nullptr, nullptr ) ;
//      
//      if( !this->format_context ) this->throwError( "Failed to open file." ) ;
//      
//      if( avformat_find_stream_info( this->format_context, nullptr ) < 0 ) this->throwError( "Failed to find any stream information." ) ;
//      
//      for( unsigned index = 0; index < this->format_context->nb_streams; index++ )
//      {
//        auto stream = this->format_context->streams[ index ] ;
//        auto codec = CodecData( stream ) ;
//        this->codecs.push_back( codec ) ;
//        if( codec.is() ){  video_index = index ; this->width = codec.width() ; this->height = codec.height() ; this->channels = codec.channels() ; } ;
//        if( codec.isAudio() ) audio_index = index ;
//      }
//    }
//    
//    auto next() -> DecodedFrame
//    {
//      if( !this->av_frame  ) this->av_frame  = av_frame_alloc () ;
//      if( !this->av_packet ) this->av_packet = av_packet_alloc() ;
//      
//      if( av_read_frame( this->format_context, this->av_packet ) >= 0 )
//      {
//        if( this->av_packet->stream_index == this->video_index && this->video_index >= 0 )
//        {
//          decode( this->av_packet, this->codecs[ this->video_index ], this->av_frame ) ;
//        }
//        
////        if( packet->stream_index == this->audio_index && this->audio_index >= 0 )
////        {
////          decodeAudio( packet, this->codecs[ this->audio_index ], frame ) ;
////        }
//      }
//      
//      this->pos++ ;
//      return DecodedFrame{ this->av_frame, this->av_packet } ;
//    }
//
//    auto infoString() -> std::string
//    {
//      std::stringstream str ;
//      
//      str << "Omega  Info\n" ;
//      str << "------------------------------------------------\n" ;
//      if( this->format_context )
//      {
//        str << "Format: " << this->format_context->iformat->long_name << ", Duration: " << this->format_context->duration << "\n" ;
//      }
//      
//      int index = 0 ;
//      for( auto& codec : this->codecs )
//      {
//        str << "Codec#: " << index << "\n" ;
//        str << "Name: " << codec.codec->long_name << " id: " << codec.codec->id << " bit_rate: " << codec.params->bit_rate << "\n" ;
//        switch( codec.params->codec_type )
//        {
//          case AVMEDIA_TYPE_VIDEO :
//            str << "-- resolution: " << codec.params->width << " " << codec.params->height << " " << codec.params->channels << "\n" ;
//            break ;
//          case AVMEDIA_TYPE_AUDIO :
//            str << "--Audio Info: Channels-> " << codec.params->channels << " | Sample Rate-> " << codec.params->sample_rate << "\n" ;
//            break ;
//          default : break ;
//        }
//        
//        str << "\n" ;
//      }
//      
//      return str.str() ;
//    }
//    
//    auto seek( int index ) -> void
//    {
//      av_seek_frame( this->format_context, -1, index, 0 ) ;
//    }
//    
//    ~LoaderData()
//    {
//      
//    }
//  };
//  
//  Loader::Loader( const char* video_path )
//  {
//    this->data = std::make_unique<LoaderData>( video_path ) ;
//  }
//  
//  Loader::~Loader()
//  {
//    
//  }
//  
//  auto Loader::info() -> std::string
//  {
//    return data->infoString() ;
//  }
//  
//  auto Loader::next() -> const Frame&
//  {
//    auto& frame = data->frame ;
//    auto next = data->next() ;
//    
//    FormatConversion conversion( static_cast<AVPixelFormat>( next.frame->format ), AVPixelFormat::AV_PIX_FMT_RGBA ) ;
//    
////static  std::vector<unsigned char> tmp ;
////    if( tmp.empty() ) tmp.resize( size ) ;
////    dst.data       = data->av_frame_output->data     ;
////    dst.width      = src.width                       ;
////    dst.height     = src.height                      ;
////    dst.line_width = data->av_frame_output->linesize ;
////    dst.line_width[ 0 ] = dst.width * 4 ;
////    
////    dst.data[ 0 ] = tmp.data() ;
////    
////    conversion.convert( next, dst ) ;
////    frame.pixels.resize( size ) ;
////    if( size != 0 )
////    {
////      std::copy( data->av_frame_output->data[ index ], data->av_frame_output->data[ index ] + size, frame.pixels.data() ) ;
////    }
////
////    frame.width    = next.frame->width    ;
////    frame.height   = next.frame->height   ;
////    frame.channels = 4 ;
//
//    return frame ;
//  }
//  
//  auto Loader::frame( int index ) -> const Frame&
//  {
//    int current = this->data->pos ;
//    this->seek( index ) ;
//    
//    const auto& frame = this->next() ;
//    this->seek( current ) ;
//    return frame ;
//  }
//  
//  auto Loader::seek( int index ) -> void
//  {
//    this->data->seek( index ) ;
//  }
//  
//  auto Loader::count() -> int
//  {
//    return data->format_context->duration ;
//  }
//  
//  auto Loader::width() -> int
//  {
//    return data->width ;
//  }
//  
//  auto Loader::height() -> int
//  {
//    return data->height ;
//  }
//  
//  auto Loader::channels() -> int
//  {
//    return data->channels ;
//  }
//}
