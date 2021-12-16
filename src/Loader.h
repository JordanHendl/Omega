#pragma once

#include <memory>
#include <vector>
#include "Structures.h"

namespace omega
{
  struct LoaderConfig
  {
    std::string path          ;
    Format      output_format ;
    
    LoaderConfig()
    {
      this->path          = ""            ;
      this->output_format = Format::RGBA8 ;
    }
  };
  
  class Loader
  {
    public:
      Loader( const LoaderConfig& cfg ) ;
      ~Loader() ;
      auto info() -> std::string ;
      auto advance() -> void ;
      auto frame() -> const Image& ;
//      auto audio() -> const Audio& ;
      auto seek( int pos ) -> void ;
      auto count() -> int ;
      auto width() -> int ;
      auto height() -> int ;
      auto channels() -> int ;
    private:
      struct LoaderData ;
      std::unique_ptr<LoaderData> data ;
  };
}

