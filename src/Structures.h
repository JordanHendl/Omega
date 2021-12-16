/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Structures.h
 * Author: jhendl
 *
 * Created on December 16, 2021, 8:54 AM
 */

#pragma once
#include <vector>

namespace omega
{
  enum class Format : int
  {
    RGBA8,
    RGBA32f,
    YUV420,
  };

  struct Image
  {
    std::vector<unsigned char> pixels ;
    int width    ;
    int height   ;
    int channels ;
    
    auto valid() -> bool
    {
      return this->width != 0 && this->height != 0 && this->channels != 0 ;
    }
  };
}
  
