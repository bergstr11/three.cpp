//
// Created by byter on 05.08.17.
//

#ifndef THREEPP_OPENGLRENDERTARGET_H
#define THREEPP_OPENGLRENDERTARGET_H

#include <memory>
#include <threepp/util/simplesignal.h>
#include <threepp/textures/DepthTexture.h>
#include <threepp/util/sole.h>
#include "Properties.h"
#include <threepp/renderers/Renderer.h>

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

class RenderTargetExternal : public RenderTarget
{
  friend class Textures;
  friend class Renderer_impl;

  class ExternalTexture : public Texture
  {
  public:
    using Ptr = std::shared_ptr<ExternalTexture>;
    const GLuint handle;
    const GLsizei width, height;

    ExternalTexture(GLuint handle, GLsizei width, GLsizei height)
       : Texture(texture::Resolver::makeNull(), Texture::options(), false, false), handle(handle), width(width), height(height)
    {}

    bool isPowerOfTwo() const override {
      return math::isPowerOfTwo(width) && math::isPowerOfTwo(height);
    }
  };

  const GLuint frameBuffer;
  const ExternalTexture::Ptr _texture;
  const CullFace _faceCulling;
  const FrontFaceDirection _faceDirection;

  bool _reuse = false;

protected:
  RenderTargetExternal(GLuint frameBuffer, GLuint texture, GLsizei width, GLsizei height,
                       CullFace faceCulling, FrontFaceDirection faceDirection,
                       bool depthBuffer, bool stencilBuffer)
     : RenderTarget(TextureTarget::twoD, width, height, depthBuffer, stencilBuffer),
       frameBuffer(frameBuffer), _texture(std::make_shared<ExternalTexture>(texture, width, height)),
       _faceCulling(faceCulling), _faceDirection(faceDirection)
  {
  }

public:
  Signal<void(RenderTargetExternal &)> onDispose;

  using Ptr = std::shared_ptr<RenderTargetExternal>;
  static Ptr make(GLuint frameBuffer, GLuint texture, GLsizei width, GLsizei height,
                  CullFace faceCulling, FrontFaceDirection faceDirection,
                  bool depthBuffer=true, bool stencilBuffer=true)
  {
    return Ptr(new RenderTargetExternal(frameBuffer, texture, width, height, faceCulling, faceDirection,
                                        depthBuffer, stencilBuffer));
  }

  bool reuse() const {return _reuse;}

  void setReuse(bool reuse) override {_reuse = reuse;}

  void init(Renderer *renderer) override;

  Texture::Ptr texture() const override {
    return _texture;
  }
  GLuint textureHandle() const {return _texture->handle;}

  void dispose() override
  {
  }
};

class RenderTargetInternal : public RenderTarget
{
  friend class Textures;
  friend class Renderer_impl;

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
  DataTexture::Ptr _texture;

  GLuint renderBuffer=0, frameBuffer=0;

  DepthTexture::Ptr _depthTexture;

protected:
  RenderTargetInternal(const Options &options, GLsizei width, GLsizei height)
     : RenderTarget(TextureTarget::twoD, width, height, options.depthBuffer, options.stencilBuffer),
       _texture(DataTexture::make(options, width, height))
  {
    _depthTexture = options.depthTexture;
  }

public:
  Signal<void(RenderTargetInternal &)> onDispose;

  using Ptr = std::shared_ptr<RenderTargetInternal>;
  static Ptr make(const Options &options, GLsizei width, GLsizei height) {
    return Ptr(new RenderTargetInternal(options, width, height));
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

  std::vector<GLuint> frameBuffers;
  std::vector<GLuint> renderBuffers;

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
#endif //THREEPP_OPENGLRENDERTARGET_H
