//
// Created by byter on 29.09.17.
//

#ifndef THREE_QT_CUBETEXTURE_H
#define THREE_QT_CUBETEXTURE_H

#include "Texture.h"
#include <array>

namespace three {

class CubeTexture :public TextureBase
{
public:
  struct Options : public TextureBase::Options {
    mapping = TextureMapping::CubeReflection;
  };

protected:
  const std::array<QImage, 6> _images;

  CubeTexture(const std::array<QImage, 6> images, const Options &options) : TextureBase(options), _images(images)
  {
    _flipY = false;
  }

public:
  using Ptr = std::shared_ptr<CubeTexture>;
  static Ptr make(const std::array<QImage, 6> images, const Options &options) {
    return Ptr(new CubeTexture(images, options));
  }

  const QImage &images(unsigned index)
  {
    return _images[index];
  }
};

}
#endif //THREE_QT_CUBETEXTURE_H
