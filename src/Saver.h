/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VideoSaver.h
 * Author: jhendl
 *
 * Created on December 16, 2021, 8:55 AM
 */

#pragma once
#include <memory>
#include <vector>

namespace omega
{
  class VideoSaver
  {
    public:
      VideoSaver( const char* path ) ;
      ~VideoSaver() ;
      auto info() -> std::string ;
//      auto next() -> const Frame& ;
//      auto frame( int index ) -> const Frame& ;
//      auto seek( int pos ) -> void ;
//      auto count() -> int ;
//      auto width() -> int ;
//      auto height() -> int ;
//      auto channels() -> int ;
    private:
      struct VideoSaverData ;
      std::unique_ptr<VideoSaverData> data ;
  };
}

