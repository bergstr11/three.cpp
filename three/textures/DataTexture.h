//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_DATATEXTURE_H
#define THREE_QT_DATATEXTURE_H

#include "Texture.h"

namespace three {

/**
 * template
 * @tparam dataCount
 */
template <unsigned dataCount>
class DataTextureT : public Texture
{
  friend class Textures;

protected:
  std::array<TextureData::Ptr, dataCount> _datas;

  size_t const _width, _height;
  bool _compressed;

  DataTextureT(texture::Resolver::Ptr resolver, const TextureOptions &options, size_t width, size_t height, bool compressed=false)
     : Texture(resolver, options, false, false, 1),
       _width(width), _height(height), _compressed(compressed)
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

  bool compressed() const {_compressed;}
  size_t width() const {return _width;}
  size_t height() const {return _height;}
  const float *data() const {return _datas[0]->data();}
  const byte *bytes() const {return _datas[0]->bytes();}

  bool isPowerOfTwo() const override {
    return math::isPowerOfTwo(_width) && math::isPowerOfTwo(_height);
  }
};

/**
 * class
 */
class DataTexture : public DataTextureT<1>
{
  DataTexture(const TextureOptions &options, const TextureData::Ptr data, size_t width, size_t height, bool compressed=false)
     : DataTextureT(texture::ResolverT<DataTexture>::make(*this), options, width, height, compressed)
  {
    _datas[0] = data;
  }

public:
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

  static Ptr make(const TextureData::Ptr data, unsigned width, unsigned height)
  {
    return Ptr(new DataTexture(DataTextureT::options(), data, width, height));
  }
};

/**
 * class
 */
class DataCubeTexture : public DataTextureT<6>
{
protected:

  DataCubeTexture(const TextureOptions &options, const std::array<TextureData::Ptr, 6> datas, size_t width, size_t height,
                  bool compressed)
     : DataTextureT(texture::ResolverT<DataCubeTexture>::make(*this), options, width, height, compressed)
  {
    _datas = datas;
  }

public:
  static TextureOptions options()
  {
    TextureOptions options = Texture::options();
    options.mapping = TextureMapping::CubeReflection;
    return options;
  }

  using Ptr = std::shared_ptr<DataCubeTexture>;
  static Ptr make(const TextureOptions &options, const std::array<TextureData::Ptr, 6> datas, size_t width, size_t height,
                  bool compressed=false) {
    return Ptr(new DataCubeTexture(options, datas, width, height, compressed));
  }

  const TextureData &data(unsigned index)
  {
    return *_datas[index];
  }

  void setData(const TextureData::Ptr &data, unsigned index) {
    _datas[index] = data;
  }
};

}
#endif //THREE_QT_DATATEXTURE_H
