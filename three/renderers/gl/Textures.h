//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_TEXTURES_H
#define THREE_QT_TEXTURES_H

#include <QOpenGLFunctions>
#include <QImage>
#include <math/Math.h>
#include <textures/CubeTexture.h>
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

  void setTextureCubeDynamic( Texture::Ptr texture, unsigned slot );
  void setTextureParameters(TextureTarget textureTarget, Texture::Ptr texture);
  void uploadTexture(Properties::Map textureProperties, Texture::Ptr texture, unsigned slot );
  void setupFrameBufferTexture(GLuint framebuffer, const RenderTarget &renderTarget, GLenum attachment, TextureTarget textureTarget);
  void setupRenderBufferStorage(GLuint renderbuffer, const RenderTarget &renderTarget );
  void setupDepthTexture(GLuint framebuffer, RenderTarget &renderTarget);
  void setupDepthRenderbuffer(RenderTarget &renderTarget);

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

  void deallocateTexture(Texture *texture);
  void deallocateRenderTarget(RenderTarget *renderTarget);
  void setTexture2D(Texture::Ptr texture, unsigned slot);
  void setTextureCube(CubeTexture::Ptr texture, unsigned slot);
  void updateRenderTargetMipmap(const Renderer::Target::Ptr &renderTarget);
  void setupRenderTarget(RenderTarget &renderTarget);

  void onRenderTargetDispose(RenderTarget *renderTarget);

};
}
}
#endif //THREE_QT_TEXTURES_H
