#pragma once

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

#include <Structures.h>
#include <map>

namespace omega
{
  struct Image ;
  struct Audio ;
  class  Frame ;

  /** Class for receiving published data through function pointers AKA 'setters'.
   */
  template<typename Type>
  class FrameSubscriber
  {
    public:
      typedef const Type& ( *Callback )( const Frame& ) ;
      
      /** Virtual deconstructor.
       */
      virtual ~FrameSubscriber() {} ;
      
      /** Method to fullfill a subscription using the input pointer.
       * @param pointer Pointer to the data requested by this object's subscription.
       * @param idx The index to use for the subscription.
       */
      virtual const Type& execute( const Frame& event ) = 0 ;
  };
  
  /** Template class to encapsulate a subscriber that recieves data via object.
   */
  template<class Object, class Type>
  class FrameMethodSubscriber : public FrameSubscriber<Type>
  {
    public:
      typedef const Type& ( Object::*Callback )( const Frame& ) ;

      FrameMethodSubscriber( Object* obj, Callback callback ) ;

      const Type& execute( const Frame& event ) ;
    private:
      Object*  object   ;
      Callback callback ;
  };
  
  /** Template class to encapsulate a subscriber that recieves data via function.
   */
  template<class Type>
  class FrameFunctionSubscriber : public FrameSubscriber<Type>
  {
    public:
      typedef const Type& (*Callback)( const Frame& ) ;

      FrameFunctionSubscriber( Callback callback ) ;
      
      const Type& execute( const Frame& event ) ;
    private:
      Callback callback ;
  };

  class ConversionCallbacks
  {
    public:
      using AudioCallback = const Audio& (*)( const Frame& ) ;
      using VideoCallback = const Image& (*)( const Frame& ) ;
      
      ConversionCallbacks( omega::Callbacks& cbs ) ;

      ~ConversionCallbacks() ;
      
      auto add( AudioCallback cb ) -> void ;
      
      template<class Object>
      auto add( Object* obj, const Image& (Object::*callback)( const Frame& ) ) -> void ;
      
      auto add( VideoCallback cb ) -> void ;
      
      template<class Object>
      auto add( Object* obj, const Audio& (Object::*callback)( const Frame& ) ) -> void ;
      
      auto pulse( AVMediaType type, const Frame& frame ) -> void ;
      
    private:
      


      omega::Callbacks*       recievers ;
      FrameSubscriber<Audio>* audio_cb  ;
      FrameSubscriber<Image>* video_cb  ;
  };
  
  template<class Object>
  auto ConversionCallbacks::add( Object* obj, const Image& (Object::*callback)( const Frame& ) ) -> void
  {
    if( this->video_cb ) delete this->video_cb ;
    video_cb = new FrameMethodSubscriber<Object, Image>( obj, callback ) ;
  }
  
  template<class Object>
  auto ConversionCallbacks::add( Object* obj, const Audio& (Object::*callback)( const Frame& ) ) -> void
  {
    if( this->audio_cb ) delete this->audio_cb ;
    audio_cb = new FrameMethodSubscriber<Object, Audio>( obj, callback ) ;
  }

  template<class Type>
  FrameFunctionSubscriber<Type>::FrameFunctionSubscriber( const Type& ( *callback )( const Frame& ) )
  {
    this->callback = callback ;
  }
  
  template<class Type>
  auto FrameFunctionSubscriber<Type>::execute( const Frame& obj ) -> const Type&
  {
    return ( this->callback )( obj ) ;
  }
  
  template<class Object, class Type>
  FrameMethodSubscriber<Object, Type>::FrameMethodSubscriber( Object* obj, const Type& (Object::*callback )( const Frame& ) )
  {
    this->object   = obj      ;
    this->callback = callback ;
  }
  
  template<class Object, class Type>
  auto FrameMethodSubscriber<Object, Type>::execute( const Frame& obj ) -> const Type&
  {
    return ( ( this->object )->*( this->callback ) )( obj ) ;
  }
}