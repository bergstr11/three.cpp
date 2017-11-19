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
private:
  const size_t width, height;

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
};

}

#endif //THREE_QT_DEPTHTEXTURE_H
