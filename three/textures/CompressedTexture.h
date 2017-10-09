//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_COMPRESSEDTEXTURE_H
#define THREE_QT_COMPRESSEDTEXTURE_H

#include "Texture.h"

namespace three {

class CompressedTexture : public TextureBase
{
protected:
  unsigned _width, _height;
  bool _mipmaps;

  CompressedTexture(bool mipmaps, unsigned width, unsigned height, const Options &options) : TextureBase(options)
  {
    _width = width;
    _height = height;
    _mipmaps = mipmaps;

    // no flipping for cube textures
    // (also flipping doesn't work for compressed textures )

    _flipY = false;

    // can't generate mipmaps for compressed textures
    // mips must be embedded in DDS files

    _generateMipmaps = false;
  }

public:
  using Ptr = std::shared_ptr<CompressedTexture>;
  static Ptr make(bool mipmaps, unsigned width, unsigned height, const Options &options) {
    return Ptr(new CompressedTexture(mipmaps, width, height, options));
  }
};

}

#endif //THREE_QT_COMPRESSEDTEXTURE_H
