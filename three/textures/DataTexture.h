//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_DATATEXTURE_H
#define THREE_QT_DATATEXTURE_H

#include "Texture.h"

namespace three {

class DataTexture : public Texture
{
protected:
  const std::vector<float> _data;
  unsigned const _width, _height;

  DataTexture(const TextureOptions &options, const std::vector<float> data, unsigned width, unsigned height)
     : Texture(texture::ResolverT<DataTexture>::make(*this), options, false, false, 1),
       _width(width), _height(height), _data(data)
  {
  }

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.minFilter = TextureFilter::Nearest;
    options.magFilter = TextureFilter::Nearest;
    return options;
  }

  using Ptr = std::shared_ptr<DataTexture>;
  static Ptr make(const TextureOptions &options,
                  const std::vector<float> data,
                  unsigned width,
                  unsigned height)
  {
    return Ptr(new DataTexture(options, data, width, height));
  }

  static Ptr make(const std::vector<float> data, unsigned width, unsigned height)
  {
    return Ptr(new DataTexture(options(), data, width, height));
  }
};

}
#endif //THREE_QT_DATATEXTURE_H
