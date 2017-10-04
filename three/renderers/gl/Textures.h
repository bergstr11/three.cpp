//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_TEXTURES_H
#define THREE_QT_TEXTURES_H

#include <QOpenGLFunctions>
#include <QImage>
#include <math/Math.h>
#include <textures/Texture.h>
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
  //bool paramThreeToGL;

public:
  Textures(QOpenGLFunctions * fn, Extensions &extensions, State &state, Properties &properties,
     Capabilities &capabilities, MemoryInfo &infoMemory)
  : _fn(fn), _extensions(extensions), _state(state), _properties(properties), _capabilities(capabilities),
    _infoMemory(infoMemory)
  {}

  QImage clampToMaxSize(const QImage &image, int maxSize )
  {
    if (image.width() > maxSize || image.height() > maxSize )
      return image.width() > image.height() ? image.scaledToWidth(maxSize) : image.scaledToHeight(maxSize);

    return image;
  }

  bool isPowerOfTwo(const QImage &image)
  {
    return math::isPowerOfTwo(image.width()) && math::isPowerOfTwo(image.height());
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
  void setTextureCube(Texture::Ptr texture, unsigned slot);

  void onTextureDispose(Texture *texture);
  void onRenderTargetDispose(RenderTarget *renderTarget);

};
}
}
#endif //THREE_QT_TEXTURES_H
