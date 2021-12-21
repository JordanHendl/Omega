/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Structures.cpp
 * Author: jhendl
 *
 * Created on December 19, 2021, 12:11 AM
 */

#include <queue>

#include "Structures.h"

namespace omega
{
  auto Container::push( const Audio& audio ) -> void
  {
//    this->m_audio.push( audio ) ;
    static std::vector<unsigned short> tmp ;
    static int timestamp = 0 ;
    
    for( auto& sh : audio.data ) tmp.push_back( sh ) ;
    if( timestamp == 0 ) timestamp = audio.timestamp ;
    if( tmp.size() > 25500 )
    {
      auto out = Audio() ;
      out.data = tmp ;
      out.sample_rate = audio.sample_rate ;
      out.timestamp = timestamp ;
      tmp.clear() ;
      timestamp = 0 ;
      this->m_audio.push( out ) ;
    }
  }
   
  auto Container::audioTime( float time ) -> bool
  {
    if( this->m_audio.empty() ) return false ;
    return time >= this->m_audio.front().timestamp ;
  }
  
  auto Container::videoTime( float time ) -> bool
  {
    if( this->m_video.empty() ) return false ;
    return time >= this->m_video.front().timestamp ;
  }

  auto Container::push( const Image& video ) -> void
  {
    this->m_video.push( video ) ;
  }
  
  auto Container::hasAudio() -> bool
  {
    return !this->m_audio.empty() ;
  }
  
  auto Container::hasVideo() -> bool
  {
    return !this->m_video.empty() ;
  }
  
  auto Container::audio() -> Audio
  {
    auto audio = this->m_audio.front() ;
    this->m_audio.pop() ;
    return audio ;
  }
  
  auto Container::video() -> Image
  {
    auto video = this->m_video.front() ;
    this->m_video.pop() ;
    return video ;
  }
  
  Callbacks::Callbacks()
  {
    this->video_cb = nullptr ;
    this->audio_cb = nullptr ;
  }
      
  Callbacks::~Callbacks()
  {
    if( this->video_cb ) delete this->video_cb ;
    if( this->audio_cb ) delete this->audio_cb ;
  }
  
  auto Callbacks::add( AudioCallback cb ) -> void
  {
    if( this->audio_cb ) delete this->audio_cb ;
    this->audio_cb = new FunctionSubscriber<Audio>( cb ) ;
  }
  
  auto Callbacks::add( VideoCallback cb ) -> void
  {
    if( this->video_cb ) delete this->video_cb ;
    this->video_cb = new FunctionSubscriber<Image>( cb ) ;
  }
  
  auto Callbacks::pulse( const Audio& audio ) -> void
  {
    if( this->audio_cb ) this->audio_cb->execute( audio ) ;
  }
  
  auto Callbacks::pulse( const Image& image ) -> void
  {
    if( this->video_cb ) this->video_cb->execute( image ) ;
  }
}
