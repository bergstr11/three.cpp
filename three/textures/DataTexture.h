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
    Options() {
      magFilter = TextureFilter ::Nearest;
      minFilter = TextureFilter ::Nearest;
    }
  };

protected:
  const std::vector<float> _data;
  unsigned const _width, _height;

  DataTexture(const std::vector<float> data, unsigned width, unsigned height, const Options &options)
     : TextureBase(options), _width(width), _height(height), _data(data)
  {
    _generateMipmaps = false;
    _flipY = false;
    _unpackAlignment = 1;
  }

public:
  using Ptr = std::shared_ptr<DataTexture>;
  static Ptr make(const std::vector<float> data,
                  unsigned width, unsigned height,
                  TextureFormat format,
                  TextureType type) {
    DataTexture::Options o;
    o.format = format;
    o.type = type;
    return Ptr(new DataTexture(data, width, height, o));
  }
};

}
#endif //THREE_QT_DATATEXTURE_H
