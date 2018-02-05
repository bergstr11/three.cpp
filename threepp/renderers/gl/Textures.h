//
// Created by byter on 02.10.17.
//

#ifndef THREEPP_TEXTURES_H
#define THREEPP_TEXTURES_H

#include <QOpenGLFunctions>
#include <QImage>
#include <threepp/math/Math.h>
#include "RenderTarget.h"
#include "Extensions.h"
#include "State.h"
#include "Properties.h"
#include "Capabilities.h"
#include "Helpers.h"

namespace three {
namespace gl
{

class Textures
{
  QOpenGLFunctions * const _fn;
  Extensions &_extensions;
  State & _state;
  Properties &_properties;
  Capabilities &_capabilities;
  MemoryInfo &_infoMemory;

  GLuint _defaultFBO = 0;

  void setTextureCubeDynamic( Texture::Ptr texture, unsigned slot );
  void setTextureParameters(TextureTarget textureTarget, Texture &texture);
  void uploadTexture(GlProperties &textureProperties, Texture &texture, unsigned slot );
  void setupFrameBufferTexture(GLuint framebuffer, const Renderer::Target &renderTarget, GLenum attachment, TextureTarget textureTarget);
  void setupRenderBufferStorage(GLuint renderbuffer, const RenderTarget &renderTarget );
  void setupDepthTexture(GLuint framebuffer, RenderTargetInternal &renderTarget);
  void setupDepthRenderbuffer(RenderTargetInternal &renderTarget);
  void setupDepthRenderbuffer(RenderTargetCube &renderTarget);

public:
  Textures(QOpenGLFunctions * fn, Extensions &extensions, State &state, Properties &properties,
     Capabilities &capabilities, MemoryInfo &infoMemory)
  : _fn(fn), _extensions(extensions), _state(state), _properties(properties), _capabilities(capabilities),
    _infoMemory(infoMemory)
  {}

  QImage clampToMaxSize(const QImage &image, int maxSize, bool flipY )
  {
    QImage img = flipY ? image.mirrored() : image;

    if (img.width() > maxSize || img.height() > maxSize )
      return img.width() > img.height() ? img.scaledToWidth(maxSize) : img.scaledToHeight(maxSize);

    return img;
  }

  QImage makePowerOfTwo(const QImage &image)
  {
    int width = math::nearestPowerOfTwo(image.width() );
    int height = math::nearestPowerOfTwo(image.height());

    return image.scaled(width, height);
  }

  inline GLenum filterFallback(TextureFilter f)
  {
    return f == TextureFilter::Nearest
           || f == TextureFilter::NearestMipMapNearest
           || f == TextureFilter::NearestMipMapLinear ? GL_NEAREST : GL_LINEAR;
  }

  void deallocateTexture(Texture &texture);
  void deallocateRenderTarget(RenderTargetInternal &renderTarget);
  void deallocateRenderTarget(RenderTargetCube &renderTarget);
  void setTexture2D(Texture::Ptr texture, unsigned slot);
  void setTextureCube(Texture::Ptr texture, unsigned slot);
  void updateRenderTargetMipmap(const RenderTarget::Ptr &renderTarget);
  void setupRenderTarget(RenderTargetExternal &renderTarget);
  void setupRenderTarget(RenderTargetInternal &renderTarget);
  void setupRenderTarget(RenderTargetCube &renderTarget);

  void onRenderTargetDispose(RenderTargetInternal &renderTarget);
  void onRenderTargetDispose(RenderTargetCube &renderTarget);
  void onTextureDispose(Texture &texture);

  void setDefaultFramebuffer(GLuint fbo) {
    _defaultFBO = fbo;
  }

  GLuint defaultFramebuffer();
};
}
}
#endif //THREEPP_TEXTURES_H
