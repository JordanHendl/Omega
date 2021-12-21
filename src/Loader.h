#pragma once

#include <memory>
#include <vector>
#include "Structures.h"

namespace omega
{
  struct LoaderConfig
  {
    std::string path                ;
    Format      output_format       ;
    bool        wait_for_next_frame ;
    bool        enable_audio        ;
    
    LoaderConfig()
    {
      this->path                = ""            ;
      this->output_format       = Format::RGBA8 ;
      this->wait_for_next_frame = true          ;
      this->enable_audio        = true          ;
    }
  };
  
  class Loader
  {
    public:
      Loader( const LoaderConfig& cfg, Callbacks& cb ) ;
      ~Loader() ;
      auto info() -> std::string ;
      auto advance() -> void ;
      auto play() -> void ;
      auto pause() -> void ;
      auto seek( int pos ) -> void ;
      auto playing() -> bool ;
      auto duration() -> float ;
      auto width() -> int ;
      auto height() -> int ;
      auto channels() -> int ;
      auto frequency() -> int ;
    private:
      struct LoaderData ;
      std::unique_ptr<LoaderData> data ;
  };
}

