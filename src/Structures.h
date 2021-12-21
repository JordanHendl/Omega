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
#include <queue>

namespace omega
{
  struct ConversionCallbacks ;
  
  enum class Format : int
  {
    RGBA8,
    RGBA32f,
    YUV420,
  };

  struct Image
  {
    std::vector<unsigned char> pixels ;
    int   width     ;
    int   height    ;
    int   channels  ;
    float timestamp ;

    Image()
    {
      this->width     = 0   ;
      this->height    = 0   ;
      this->channels  = 0   ;
      this->timestamp = 0.f ;
    }

    auto valid() const -> bool
    {
      return this->width != 0 && this->height != 0 && this->channels != 0 ;
    }
  };
  
  struct Audio
  {
    std::vector<unsigned short> data ;
    int   sample_rate ;
    float timestamp   ;
    
    Audio()
    {
      this->sample_rate = 0 ;
      this->timestamp   = 0 ;
    }

    auto valid() const -> bool 
    {
      return !this->data.empty() && this->sample_rate != 0 ;
    }
  };
  
  struct Container
  {
    public:
      auto push( const Audio& audio ) -> void ;
      auto push( const Image& audio ) -> void ;
      auto audioTime( float time ) -> bool ;
      auto videoTime( float time ) -> bool ;
      auto hasAudio() -> bool ;
      auto hasVideo() -> bool ;
      auto audio() -> Audio ;
      auto video() -> Image ;
    private:
      std::queue<Audio> m_audio ;
      std::queue<Image> m_video ;
  };
  
  /** Class for receiving published data through function pointers AKA 'setters'.
   */
  template<typename Type>
  class Subscriber
  {
    public:
      typedef void ( *Callback )( const Type& ) ;
      
      /** Virtual deconstructor.
       */
      virtual ~Subscriber() {} ;
      
      /** Method to fullfill a subscription using the input pointer.
       * @param pointer Pointer to the data requested by this object's subscription.
       * @param idx The index to use for the subscription.
       */
      virtual void execute( const Type& event ) = 0 ;
  };
  
  /** Template class to encapsulate a subscriber that recieves data via object.
   */
  template<class Object, class Type>
  class MethodSubscriber : public Subscriber<Type>
  {
    public:
      typedef void ( Object::*Callback )( const Type& ) ;

      MethodSubscriber( Object* obj, Callback callback ) ;

      void execute( const Type& event ) ;
    private:
      Object*  object   ;
      Callback callback ;
  };
  
  /** Template class to encapsulate a subscriber that recieves data via function.
   */
  template<class Type>
  class FunctionSubscriber : public Subscriber<Type>
  {
    public:
      typedef void (*Callback)( const Type& ) ;

      FunctionSubscriber( Callback callback ) ;
      
      void execute( const Type& event ) ;
    private:
      Callback callback ;
  };
  
  class Callbacks
  {
    using AudioCallback = void (*)( const Audio& ) ;
    using VideoCallback = void (*)( const Image& ) ;
    
    public:
      Callbacks() ;
      
      ~Callbacks() ;
      
      auto add( AudioCallback cb ) -> void ;
      
      template<class Object>
      auto add( Object* obj, void (Object::*callback)( const Audio& ) ) -> void ;
      
      auto add( VideoCallback cb ) -> void ;
      
      template<class Object>
      auto add( Object* obj, void (Object::*callback)( const Image& ) ) -> void ;
      
      auto pulse( const Audio& audio ) -> void ;
      
      auto pulse( const Image& image ) -> void ;
    private:
      friend class omega::ConversionCallbacks ;

      Subscriber<Audio>* audio_cb ;
      Subscriber<Image>* video_cb ;
  };
  
  template<class Object>
  auto Callbacks::add( Object* obj, void (Object::*callback)( const Image& ) ) -> void
  {
    if( this->video_cb ) delete this->video_cb ;
    this->video_cb = new MethodSubscriber<Object, Image>( obj, callback ) ;
  }
  
  template<class Object>
  auto Callbacks::add( Object* obj, void (Object::*callback)( const Audio& ) ) -> void
  {
    if( this->audio_cb ) delete this->audio_cb ;
    this->audio_cb = new MethodSubscriber<Object, Audio>( obj, callback ) ;
  }

  template<class Type>
  FunctionSubscriber<Type>::FunctionSubscriber( void ( *callback )( const Type& ) )
  {
    this->callback = callback ;
  }
  
  template<class Type>
  void FunctionSubscriber<Type>::execute( const Type& obj )
  {
    ( this->callback )( obj ) ;
  }
  
  template<class Object, class Type>
  MethodSubscriber<Object, Type>::MethodSubscriber( Object* obj, void (Object::*callback )( const Type& ) )
  {
    this->object   = obj      ;
    this->callback = callback ;
  }
  
  template<class Object, class Type>
  void MethodSubscriber<Object, Type>::execute( const Type& obj )
  {
   ( ( this->object )->*( this->callback ) )( obj ) ;
  }
}
  
