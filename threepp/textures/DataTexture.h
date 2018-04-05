//
// Created by byter on 07.10.17.
//

#ifndef THREEPP_DATATEXTURE_H
#define THREEPP_DATATEXTURE_H

#include "Texture.h"

namespace three {

/**
 * class DataTexture
 */
class DataTexture : public Texture
{
  friend class Textures;

protected:
  TextureData::Ptr _data;

  size_t const _width, _height;
  bool _compressed;

  DataTexture(const TextureOptions &options, const TextureData::Ptr data, size_t width, size_t height, bool compressed=false)
     : Texture(texture::ResolverT<DataTexture>::make(*this), options, false, 1),
       _data(data), _width(width), _height(height), _compressed(compressed)
  {
    needsUpdate();
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
                  const std::vector<byte> &data,
                  unsigned width,
                  unsigned height)
  {
    return Ptr(new DataTexture(options, TextureDataT<byte>::make(data, width, height), width, height));
  }

  static Ptr make(const TextureOptions &options,
                  const std::vector<float> &data,
                  unsigned width,
                  unsigned height)
  {
    return Ptr(new DataTexture(options, TextureDataT<float>::make(data, width, height), width, height));
  }

  static Ptr make(const TextureOptions &options, unsigned width, unsigned height)
  {
    switch(options.type) {
      case TextureType::Byte:
      case TextureType::UnsignedByte:
        return Ptr(new DataTexture(options, TextureDataT<uint8_t>::make(width, height), width, height));
      case TextureType::Float:
        return Ptr(new DataTexture(options, TextureDataT<float>::make(width, height), width, height));
      default:
        throw std::invalid_argument("type not currently implemented");
    }
  }

  bool compressed() const {return _compressed;}
  size_t width() const {return _width;}
  size_t height() const {return _height;}
  const byte *bytes() const {return _data->bytes();}

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }
};

/**
 * class DataCubeTexture
 */
class DataCubeTexture : public CubeTexture
{
  std::array<TextureData::Ptr, CubeTexture::num_faces> _datas;

protected:
  DataCubeTexture(const TextureOptions &options, const std::array<TextureData::Ptr, CubeTexture::num_faces> &datas,
                  size_t width, size_t height, bool compressed)
     : CubeTexture(texture::ResolverT<DataCubeTexture>::make(*this), options, false, 1),
       _datas(datas), _width(width), _height(height), _compressed(compressed)
  {
  }

  size_t const _width, _height;
  bool _compressed;

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.mapping = TextureMapping::CubeReflection;
    return options;
  }

  using Ptr = std::shared_ptr<DataCubeTexture>;
  static Ptr make(const TextureOptions &options, const std::array<TextureData::Ptr, 6> &datas, size_t width, size_t height,
                  bool compressed=false) {
    return Ptr(new DataCubeTexture(options, datas, width, height, compressed));
  }

  bool dontFlip() const override {return true;}

  bool isPowerOfTwo() const override
  {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }

  const TextureData &data(unsigned index)
  {
    return *_datas[index];
  }

  void setData(TextureData::Ptr data, unsigned index) {
    _datas[index] = data;
  }

  bool compressed() const {_compressed;}
  size_t width() const {return _width;}
  size_t height() const {return _height;}
};

}
#endif //THREEPP_DATATEXTURE_H
