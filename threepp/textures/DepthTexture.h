//
// Created by byter on 11/19/17.
//
#ifndef THREEPP_DEPTHTEXTURE_H
#define THREEPP_DEPTHTEXTURE_H

#include "Texture.h"

namespace three
{

class DepthTexture :public Texture
{
  friend class Textures;
private:
  size_t _width, _height;

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

  size_t &width() {return _width;}
  size_t &height() {return _height;}

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }
};

}

#endif //THREEPP_DEPTHTEXTURE_H
