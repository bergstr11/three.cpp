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

class Textures;
class Renderer_impl;

class RenderTarget : public Renderer::Target
{
protected:
  bool _depthBuffer;
  bool _stencilBuffer;

  RenderTarget(TextureTarget textureTarget, GLsizei width, GLsizei height, bool depthBuffer, bool stencilBuffer)
     : Renderer::Target(width, height), textureTarget(textureTarget), _depthBuffer(depthBuffer), _stencilBuffer(stencilBuffer)
  {}

public:
  const TextureTarget textureTarget;
  using Ptr = std::shared_ptr<RenderTarget>;

  bool depthBuffer() const {return _depthBuffer;}
  bool stencilBuffer() const {return _stencilBuffer;}

  virtual void dispose() = 0;
};

class RenderTargetExternal : public RenderTarget
{
  friend Textures;
  friend Renderer_impl;

  const GLuint textureHandle;
  const GLuint frameBuffer;
  const RenderTexture::Ptr _texture;
  const CullFace _faceCulling;
  const FrontFaceDirection _faceDirection;

protected:
  RenderTargetExternal(GLuint frameBuffer, GLuint texture, GLsizei width, GLsizei height,
                       CullFace faceCulling, FrontFaceDirection faceDirection,
                       bool depthBuffer, bool stencilBuffer)
     : RenderTarget(TextureTarget::twoD, width, height, depthBuffer, stencilBuffer),
       frameBuffer(frameBuffer), textureHandle(texture),
       _texture(RenderTexture::make(RenderTexture::options(), width, height)),
       _faceCulling(faceCulling), _faceDirection(faceDirection)
  {
  }

public:
  Signal<void(RenderTargetExternal &)> onDispose;

  using Ptr = std::shared_ptr<RenderTargetExternal>;
  static Ptr make(GLuint frameBuffer, GLuint texture, GLsizei width, GLsizei height,
                  CullFace faceCulling, FrontFaceDirection faceDirection)
  {
    return Ptr(new RenderTargetExternal(frameBuffer, texture, width, height, faceCulling, faceDirection,
                                        false, false));
  }

  void init(Renderer *renderer) override;

  Texture::Ptr texture() const override {
    return _texture;
  }

  void dispose() override
  {
  }
};

class RenderTargetInternal : public RenderTarget
{
  friend Textures;
  friend Renderer_impl;

public:
  struct Options : public TextureOptions
  {
    bool depthBuffer = true;
    bool stencilBuffer = true;
    DepthTexture::Ptr depthTexture;
    bool generateMipmaps = false;

    Options() {
      minFilter = TextureFilter::Linear;
    }
  };

private:
  GLuint renderBuffer=0, frameBuffer=0;

  RenderTexture::Ptr _texture;
  DepthTexture::Ptr _depthTexture;

protected:
  RenderTargetInternal(const Options &options, GLsizei width, GLsizei height)
     : RenderTarget(TextureTarget::twoD, width, height, options.depthBuffer, options.stencilBuffer),
       _texture(RenderTexture::make(options, width, height)), _depthTexture(options.depthTexture)
  {
    _texture->setGenerateMipmaps(options.generateMipmaps);
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
  friend Textures;
  friend Renderer_impl;

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
