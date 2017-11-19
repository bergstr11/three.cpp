//
// Created by byter on 29.09.17.
//

#ifndef THREE_QT_CUBETEXTURE_H
#define THREE_QT_CUBETEXTURE_H

#include "Texture.h"
#include <array>

namespace three {

class CubeTexture : public Texture
{
protected:
  const std::array<QImage, 6> _images;

  CubeTexture(const TextureOptions &options, const std::array<QImage, 6> images)
     : Texture(texture::ResolverT<CubeTexture>::make(*this), options), _images(images)
  {
  }

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.mapping = TextureMapping::CubeReflection;
    return options;
  }

  using Ptr = std::shared_ptr<CubeTexture>;
  static Ptr make(const TextureOptions &options, const std::array<QImage, 6> images) {
    return Ptr(new CubeTexture(options, images));
  }

  const QImage &images(unsigned index)
  {
    return _images[index];
  }
};

}
#endif //THREE_QT_CUBETEXTURE_H
