//
// Created by byter on 11/19/17.
//

#include "Texture.h"

#ifndef THREE_QT_DEPTHTEXTURE_H
#define THREE_QT_DEPTHTEXTURE_H

namespace three
{

class DepthTexture :public Texture
{
  friend class Textures;
private:
  const size_t _width, _height;

protected:
  DepthTexture(const TextureOptions &options, size_t width, size_t height);

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.format = TextureFormat::Depth;
    options.type = TextureType::UnsignedShort;
    options.minFilter = TextureFilter::Nearest;
    options.magFilter = TextureFilter::Nearest;
    return options;
  }

  using Ptr = std::shared_ptr<DepthTexture>;
  static Ptr make(const TextureOptions &options, size_t width, size_t height) {
    return Ptr(new DepthTexture(options, width, height));
  }

  size_t width() const {return _width;}
  size_t height() const {return _height;}

  bool isPowerOfTwo() override {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }
};

}

#endif //THREE_QT_DEPTHTEXTURE_H
