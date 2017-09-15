//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_OPENGLRENDERTARGET_H
#define THREE_QT_OPENGLRENDERTARGET_H

#include <memory>
#include <helper/simplesignal.h>
#include <textures/Texture.h>

namespace three {
namespace gl {

class RenderTarget
{
public:
  struct Options : public Texture::Options
  {
    bool depthBuffer = true;
    bool stencilBuffer = true;
    Texture::Ptr depthTexture;

    Options() {
      minFilter = TextureFilter::Linear;
    }
  };

private:
  unsigned _width;
  unsigned _height;

  math::Vector4 _scissor;
  bool _scissorTest = false;

  math::Vector4 _viewport;

  Texture::Ptr _texture;

  bool _depthBuffer;
  bool _stencilBuffer;
  Texture::Ptr _depthTexture;

  RenderTarget(unsigned width, unsigned height, const Options &options)
     : _width(width), _height(height), _scissor(0, 0, width, height), _viewport(0, 0, width, height)
  {
    _texture = Texture::make(options);

    _depthBuffer = options.depthBuffer;
    _stencilBuffer = options.stencilBuffer;
    _depthTexture = options.depthTexture;
  }

public:
  Signal<void()> onDispose;

  using Ptr = std::shared_ptr<RenderTarget>;
  static Ptr make(unsigned width, unsigned height, const Options &options) {
    return Ptr(new RenderTarget(width, height, options));
  }

  RenderTarget &setSize(unsigned width, unsigned height )
  {
    if ( _width != width || _height != height ) {

      _width = width;
      _height = height;

      onDispose.emitSignal();
    }

    _viewport.set( 0, 0, width, height );
    _scissor.set( 0, 0, width, height );
  }
};

}
}
#endif //THREE_QT_OPENGLRENDERTARGET_H
