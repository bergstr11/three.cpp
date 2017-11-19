//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_OPENGLRENDERTARGET_H
#define THREE_QT_OPENGLRENDERTARGET_H

#include <memory>
#include <helper/simplesignal.h>
#include <textures/DepthTexture.h>
#include <helper/sole.h>
#include "../Renderer.h"

namespace three {
namespace gl {

class RenderTarget : public Renderer::Target
{
public:
  struct Options : public TextureOptions
  {
    bool depthBuffer = true;
    bool stencilBuffer = true;
    DepthTexture::Ptr depthTexture;

    Options() {
      minFilter = TextureFilter::Linear;
    }
  };

  const sole::uuid uuid;

private:
  float _width;
  float _height;

  math::Vector4 _scissor;
  bool _scissorTest = false;

  math::Vector4 _viewport;

  bool _depthBuffer;
  bool _stencilBuffer;
  DepthTexture::Ptr _depthTexture;

  static TextureOptions textureOptions()
  {
    TextureOptions ops = DefaultTexture::options();
  }

protected:
  RenderTarget(const Options &options, float width, float height)
     : Renderer::Target(DefaultTexture::make(options)),
       _width(width), _height(height), _scissor(0, 0, width, height), _viewport(0, 0, width, height), uuid(sole::uuid0())
  {
    _depthBuffer = options.depthBuffer;
    _stencilBuffer = options.stencilBuffer;
    _depthTexture = options.depthTexture;
  }

public:
  Signal<void(RenderTarget *)> onDispose;

  using Ptr = std::shared_ptr<RenderTarget>;
  static Ptr make(const Options &options, float width, float height) {
    return Ptr(new RenderTarget(options, width, height));
  }

  const math::Vector4 &scissor() const override {return _scissor;}
  bool scissorTest() const override {return _scissorTest;}
  const math::Vector4 &viewport() const override {return _viewport;}

  DepthTexture::Ptr depthTexture() {return _depthTexture;}

  RenderTarget &setSize(float width, float height )
  {
    if ( _width != width || _height != height ) {

      _width = width;
      _height = height;

      onDispose.emitSignal(this);
    }

    _viewport.set( 0, 0, width, height );
    _scissor.set( 0, 0, width, height );
  }

  virtual int bufCount() {
    return 1;
  }
};

}
}
#endif //THREE_QT_OPENGLRENDERTARGET_H
