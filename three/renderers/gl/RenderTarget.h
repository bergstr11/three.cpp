//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_OPENGLRENDERTARGET_H
#define THREE_QT_OPENGLRENDERTARGET_H

#include <memory>
#include <helper/simplesignal.h>
#include <textures/DepthTexture.h>
#include <helper/sole.h>
#include "Properties.h"
#include "../Renderer.h"

namespace three {
namespace gl {

class RenderTarget : public Renderer::Target
{
protected:
  bool _depthBuffer;
  bool _stencilBuffer;

  RenderTarget(TextureTarget textureTarget, GLsizei width, GLsizei height, bool depthBuffer, bool stencilBuffer)
     : Renderer::Target(width, height), textureTarget(textureTarget), _depthBuffer(depthBuffer), _stencilBuffer(stencilBuffer) {}

public:
  const TextureTarget textureTarget;
  using Ptr = std::shared_ptr<RenderTarget>;

  bool depthBuffer() const {return _depthBuffer;}
  bool stencilBuffer() const {return _stencilBuffer;}

  virtual void dispose() = 0;
};

class RenderTargetDefault : public RenderTarget
{
  friend class Textures;
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

private:
  ImageTexture::Ptr _texture;

  GLuint renderBuffer, frameBuffer;

  DepthTexture::Ptr _depthTexture;

  static TextureOptions textureOptions()
  {
    TextureOptions ops = ImageTexture::options();
  }

protected:
  RenderTargetDefault(const Options &options, GLsizei width, GLsizei height, bool depthBuffer, bool stencilBuffer)
     : RenderTarget(TextureTarget::twoD, width, height, depthBuffer, stencilBuffer), _texture(ImageTexture::make(options))
  {
    _depthBuffer = options.depthBuffer;
    _stencilBuffer = options.stencilBuffer;
    _depthTexture = options.depthTexture;
  }

public:
  Signal<void(RenderTargetDefault &)> onDispose;

  using Ptr = std::shared_ptr<RenderTargetDefault>;
  static Ptr make(const Options &options, GLsizei width, GLsizei height, bool depthBuffer=true, bool stencilBuffer=true) {
    return Ptr(new RenderTargetDefault(options, width, height, depthBuffer, stencilBuffer));
  }

  Texture::Ptr texture() const override {return _texture;}
  const DepthTexture::Ptr depthTexture() const {return _depthTexture;}

  void dispose() override
  {
    if (_depthTexture) {
      _depthTexture->dispose();
    }
  }
};

class RenderTargetCube : public RenderTarget
{
  friend class Textures;
  friend class Renderer_impl;

  unsigned activeCubeFace = 0; // PX 0, NX 1, PY 2, NY 3, PZ 4, NZ 5
  unsigned activeMipMapLevel = 0;

  std::array<GLuint, 6> frameBuffers;
  std::array<GLuint, 6> renderBuffers;

protected:
  RenderTargetCube(GLsizei width, GLsizei height, bool depthBuffer, bool stencilBuffer)
     : RenderTarget(TextureTarget::cubeMap, width, height, depthBuffer, stencilBuffer)
  {}

public:
  Signal<void(RenderTargetCube &)> onDispose;

  using Ptr = std::shared_ptr<RenderTargetCube>;
  static Ptr make(GLsizei width, GLsizei height, bool depthBuffer = true, bool stencilBuffer = true) {
    return Ptr(new RenderTargetCube(width, height, depthBuffer, stencilBuffer));
  }

  Texture::Ptr texture() const override {return nullptr;}

  void dispose() override
  {
  }
};

}
}
#endif //THREE_QT_OPENGLRENDERTARGET_H
