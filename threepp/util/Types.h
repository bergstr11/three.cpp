//
// Created by byter on 11/17/17.
//

#ifndef THREEPP_TYPES_H
#define THREEPP_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <type_traits>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>

namespace three {

using byte = unsigned char;

template <typename T>
class optional {
  T _t;
  bool _isSet;

public:
  optional() : _isSet(false) {}

  template <typename F>
  optional(F&& f) : _t(f), _isSet(true) {}

  template <typename F>
  optional &operator =(F&& f) {
    _t = f;
    _isSet = true;
  }

  const T &get() const {
    return _t;
  }

  operator const T &() const {
    return _t;
  }

  bool isSet() {return _isSet;}
};

struct Mipmap {
  std::vector<unsigned char> data;
  int width, height;
};

class TextureData
{
protected:
  std::vector<Mipmap> _mipmaps;

  size_t _width = 0;
  size_t _height = 0;

  TextureData(size_t width, size_t height) : _width(width), _height(height) {}
  TextureData() = default;

public:
  using Ptr = std::shared_ptr<TextureData>;

  size_t width() const {return _width;}
  size_t height() const {return _height;}

  const std::vector<Mipmap> &mipmaps() const {return _mipmaps;}
  const Mipmap &mipmap(unsigned index) const {return _mipmaps.at(index);}

  virtual const byte *bytes() const = 0;
};

template <typename T>
class TextureDataT : public TextureData
{
  std::vector<T> _data;

  TextureDataT(const std::vector<T> &data, size_t width, size_t height)
     : TextureData(width, height), _data(data) {}

  TextureDataT(size_t width, size_t height, bool resize=false)
     : TextureData(width, height)
  {
    if(resize) _data.resize(width * height);
  }

  TextureDataT() = default;
public:
  static Ptr make(const std::vector<T> &data, size_t width, size_t height) {
    return Ptr(new TextureDataT(data, width, height));
  }

  static Ptr make(size_t width, size_t height) {
    return Ptr(new TextureDataT(width, height));
  }

  const byte *bytes() const override {
    return (byte *)_data.data();
  }
};

class Layers
{
  unsigned int mask = 1;

public:

  void set(unsigned int channel) {
    mask = (unsigned)1 << channel;
  }

  void enable(unsigned int channel) {
    mask |= 1 << channel;
  }

  void toggle(unsigned int channel) {
    mask ^= 1 << channel;
  }

  void disable(unsigned int channel) {
    mask &= ~(1 << channel);
  }

  bool test(const Layers &layers) const {
    return (mask & layers.mask) != 0;
  }
};

struct Group {
  uint32_t start;
  uint32_t count;
  uint32_t materialIndex;

  Group(uint32_t start, uint32_t count, uint32_t materialIndex)
     : start(start), count(count), materialIndex(materialIndex) {}

  Group() : start(0), count(0), materialIndex(0) {}
};

class UV : public math::Vector2
{
public:

  UV(float u = 0, float v = 0) : math::Vector2( u, v ) { }
  UV( const UV& uv ) : math::Vector2(uv) {}

  UV& lerpSelf( const UV& uv, float alpha )
  {
    _x += ( uv._x - _x ) * alpha;
    _y += ( uv._y - _y ) * alpha;

    return *this;
  }

  float &u() {return _x;}
  float &v() {return _y;}

  const float u() const {return _x;}
  const float v() const {return _y;}
};

using UV_Array = std::array<UV, 3>;

struct UpdateRange
{
  size_t offset;
  int count;

  UpdateRange(size_t offset=0, int count=-1) : offset(offset), count(count) {}
};

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * use this as hash functor for enum-key maps, or use the unordered_map further below
 */
struct EnumHash
{
  template <typename EnumT>
  typename std::underlying_type<EnumT>::type operator()(EnumT t) const
  {
    return static_cast<typename std::underlying_type<EnumT>::type>(t);
  }
};

/**
 * if your compiler doesn't accept enums as keys, you may try this..
 */
template <typename EnumT, typename ValueT>
using enum_map = std::unordered_map<EnumT, ValueT, EnumHash>;

} // namespace three

#endif //THREEPP_TYPES_H
