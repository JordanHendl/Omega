#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

namespace omega
{
  class Frame
  {
    public:
      Frame() ;
      
      Frame( Frame&& frame ) ;
      
      virtual ~Frame() ;
      
      virtual auto operator=( Frame&& frame ) -> Frame& ;
      
      virtual auto av() -> AVFrame& ;
      
      virtual auto av() const -> const AVFrame& ;
      
      virtual auto operator->() -> AVFrame* ;
      
      virtual auto operator->() const -> const AVFrame* ;
      
      virtual auto operator*() -> AVFrame& ;
      
      virtual auto operator*() const -> const AVFrame& ;
    protected:
      AVFrame* frame ;
  };
  
  
  class AudioFrame : public Frame
  {
    public:
      AudioFrame() : Frame() {} ;
      
      AudioFrame( AVSampleFormat format, int channel_layout, unsigned num_samples ) : Frame()
      {
        constexpr auto MAX_SAMPLES = 2048 ;
        
        *this->frame = {} ;
        
        this->frame->format         = static_cast<int>( format ) ;
        this->frame->sample_rate    = num_samples                ;
        this->frame->channel_layout = channel_layout             ;
        
        auto channels         = this->channels() ;
        this->frame->channels = channels         ;
        
        this->frame->linesize[ 0 ] = MAX_SAMPLES * channels ;
        this->frame->extended_data = new uint8_t*[ AV_NUM_DATA_POINTERS ] ;
        for( int index = 0; index < channels; index++ )
        {
          this->frame->data         [ index ] = new uint8_t[ MAX_SAMPLES * channels ] ;
          this->frame->extended_data[ index ] = this->frame->data[ index ] ;
        }
        
        this->frame->nb_samples = 0 ;
      } ;
      
      virtual ~AudioFrame() ;
      
      virtual auto operator=( AudioFrame&& frame ) -> AudioFrame&
      {
        auto* tmp = this->frame ;
        this->frame = frame.frame ;
        frame.frame = tmp         ;
        
        return *this ;
      }
      auto channels() -> int
      {
        return av_get_channel_layout_nb_channels( this->frame->channel_layout ) ;
      }
  };
}

