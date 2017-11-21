//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_COMPRESSEDTEXTURE_H
#define THREE_QT_COMPRESSEDTEXTURE_H

#include "Texture.h"

namespace three {

// no flipping for cube textures
// (also flipping doesn't work for compressed textures )
//
// can't generate mipmaps for compressed textures
// mips must be embedded in DDS files
class CompressedTexture : public Texture
{
protected:
  unsigned _width, _height;

  CompressedTexture(const TextureOptions &options, unsigned width, unsigned height)
     : Texture(texture::ResolverT<CompressedTexture>::make(*this), options, false, false),
       _width(width), _height(height)
  {
  }

public:
  using Ptr = std::shared_ptr<CompressedTexture>;
  static Ptr make(const TextureOptions &options, unsigned width, unsigned height) {
    return Ptr(new CompressedTexture(options, width, height));
  }

  bool isPowerOfTwo() override {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }
};

}

#endif //THREE_QT_COMPRESSEDTEXTURE_H
