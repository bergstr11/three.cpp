//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_DATATEXTURE_H
#define THREE_QT_DATATEXTURE_H

#include "Texture.h"

namespace three {

class DataTexture : public TextureBase
{
public:
  struct Options : public TextureBase::Options {
    magFilter = TextureFilter ::Nearest;
    minFilter = TextureFilter ::Nearest;
  };

protected:
  const std::vector<float> _data;

  DataTexture(const std::vector<float> data, const Options &options) : TextureBase(options), _data(data)
  {
    _generateMipmaps = false;
    _flipY = false;
    _unpackAlignment = 1;
  }

public:
  using Ptr = std::shared_ptr<DataTexture>;
  static Ptr make(const std::vector<float> data, const Options &options) {
    return Ptr(new DataTexture(data, options));
  }
};

}
#endif //THREE_QT_DATATEXTURE_H
