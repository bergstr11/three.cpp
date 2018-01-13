//
// Created by byter on 23.08.17.
//

#ifndef THREE_QT_BUFFERATTRIBUTE_H
#define THREE_QT_BUFFERATTRIBUTE_H

#include <vector>
#include <cstring>
#include <core/Color.h>
#include <memory>

#include <qopengl.h>

#include <Constants.h>
#include <helper/sole.h>
#include <helper/simplesignal.h>
#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <math/Box3.h>
#include <helper/Types.h>
#include <helper/Resolver.h>

namespace three {

template<typename T>
struct Cpp2GL;

template<>
struct Cpp2GL<float> {
  static const GLenum glEnum = GL_FLOAT;
};
template<>
struct Cpp2GL<uint16_t> {
  static const GLenum glEnum = GL_UNSIGNED_SHORT;
};
template<>
struct Cpp2GL<int16_t> {
  static const GLenum glEnum = GL_SHORT;
};
template<>
struct Cpp2GL<uint32_t> {
  static const GLenum glEnum = GL_UNSIGNED_INT;
};
template<>
struct Cpp2GL<int32_t> {
  static const GLenum glEnum = GL_INT;
};
template<>
struct Cpp2GL<int8_t> {
  static const GLenum glEnum = GL_BYTE;
};
template<>
struct Cpp2GL<uint8_t> {
  static const GLenum glEnum = GL_UNSIGNED_BYTE;
};

struct Index
{
  const uint32_t a, b, c;

  Index(uint32_t a, uint32_t b, uint32_t c) : a(a), b(b), c(c) {}
};

/**
 * a buffer attribute associates a given buffer with a geometry attribute
 */
class BufferAttribute
{
protected:
  unsigned _version = 0;

  unsigned _itemSize;
  bool _normalized;

  bool _dynamic = false;
  UpdateRange _updateRange;

  explicit BufferAttribute(bufferattribute::Resolver::Ptr resolver, unsigned itemSize, bool normalized)
     : uuid(sole::uuid0()), resolver(resolver), _itemSize(itemSize), _normalized(normalized)
  {}

  BufferAttribute(unsigned itemSize, bool normalized, bool dynamic)
     : uuid(sole::uuid0()), resolver(bufferattribute::Resolver::makeNull()), _itemSize(itemSize),
       _normalized(normalized), _dynamic(dynamic)
  {}

public:
  virtual ~BufferAttribute() = default;

  using Ptr = std::shared_ptr<BufferAttribute>;

  const bufferattribute::Resolver::Ptr resolver;

  Signal<void(const BufferAttribute &)> onUpload;

  const sole::uuid uuid;

  void setDynamic(bool value)
  {
    _dynamic = value;
  }

  void needsUpdate() {_version++;}
  bool needsUpdate() const {return _version > 0;}

  unsigned version() const {return _version;}

  bool normalized() const {return _normalized;}

  UpdateRange &updateRange() {return _updateRange;}

  bool dynamic() const {return _dynamic;}

  unsigned itemSize() const {return _itemSize;}

  virtual const void *data(size_t offset) const = 0;

  virtual GLenum glType() const = 0;

  virtual size_t byteCount() const = 0;

  virtual unsigned bytesPerElement() const = 0;
};

/**
 * a typed buffer attribute
 *
 * @tparam Type
 */
template <typename Type>
class BufferAttributeT : public BufferAttribute
{
protected:
  Type *_data;
  size_t _size = 0;

  explicit BufferAttributeT(bufferattribute::Resolver::Ptr resolver, unsigned itemSize, bool normalized)
  : BufferAttribute(resolver, itemSize, normalized)
  {}

  BufferAttributeT(unsigned itemSize, bool normalized)
     : BufferAttribute(itemSize, normalized, false)
  {}

public:
  using Ptr = std::shared_ptr<BufferAttributeT<Type>>;

  const Type &tdata(uint32_t offset) {return _data[offset];}

  const Type *tdata() {return _data;}

  GLenum glType() const override {return Cpp2GL<Type>::glEnum;}

  unsigned bytesPerElement() const override {return sizeof(Type);}

  size_t size() const {return _size;}

  size_t itemCount() const {return _size / _itemSize;}

  const Type operator [] (size_t index) {return _data[index];}

  BufferAttributeT &set(Type value, size_t offset=0)
  {
    _data[offset] = value;

    return *this;
  }

  Type get_x(size_t index) const
  {
    return _data[ index * _itemSize ];
  }

  BufferAttributeT &set_x(size_t index, Type x)
  {
    _data[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index) const
  {
    return _data[ index * _itemSize + 1 ];
  }

  BufferAttributeT &set_y(size_t index, Type y)
  {
    _data[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type get_z(size_t index) const
  {
    return _data[ index * _itemSize + 2 ];
  }

  BufferAttributeT &set_z(size_t index, Type z)
  {
    _data[ index * _itemSize + 2 ] = z;

    return *this;
  }

  Type get_w(size_t index) const
  {
    return _data[ index * _itemSize + 3 ];
  }

  BufferAttributeT &set_w(size_t index, Type w)
  {
    _data[ index * _itemSize + 3 ] = w;

    return *this;
  }

  void setXY(size_t index, Type x, Type y)
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
  }

  void setXYZ(size_t index, Type x, Type y, Type z )
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
    _data[ index + 2 ] = z;
  }

  void setXYZW(size_t index, Type x, Type y, Type z, Type w)
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
    _data[ index + 2 ] = z;
    _data[ index + 3 ] = w;
  }

  math::Box3 box3()
  {
    Type minX = std::numeric_limits<Type>::infinity();
    Type minY = std::numeric_limits<Type>::infinity();
    Type minZ = std::numeric_limits<Type>::infinity();

    Type maxX = - std::numeric_limits<Type>::infinity();
    Type maxY = - std::numeric_limits<Type>::infinity();
    Type maxZ = - std::numeric_limits<Type>::infinity();

    for(size_t i = 0; i < _size; i++) {
      Type x = get_x(i);
      Type y = get_y(i);
      Type z = get_z(i);

      if ( x < minX ) minX = x;
      if ( y < minY ) minY = y;
      if ( z < minZ ) minZ = z;

      if ( x > maxX ) maxX = x;
      if ( y > maxY ) maxY = y;
      if ( z > maxZ ) maxZ = z;
    }

    return math::Box3(math::Vector3(minX, minY, minZ), math::Vector3(maxX, maxY, maxZ));
  }

  void apply(const math::Matrix4 &matrix)
  {
    for(size_t i = 0; i < _size; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }

  void apply(const math::Matrix3 &matrix)
  {
    for(size_t i = 0; i < _size; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }

  BufferAttributeT &copyAt(unsigned dstIndex, const BufferAttributeT &srcAttribute, unsigned srcIndex)
  {
    std::memcpy(_data + dstIndex * _itemSize, srcAttribute._data + srcIndex * srcAttribute._itemSize, _itemSize);
    return *this;
  }

  size_t byteCount() const override {return _size * sizeof(Type);}

  const void *data(size_t offset) const override {return _data+offset;}
};

/**
 * a buffer attribute that manages an internal buffer
 *
 * @tparam Type
 */
template <typename Type>
class DefaultBufferAttribute : public BufferAttributeT<Type>
{
  using Super = BufferAttributeT<Type>;
  using Base = BufferAttribute;

  std::vector<Type> _array;

protected:
  DefaultBufferAttribute(const std::vector<Type> &array, unsigned itemSize, bool normalized)
     : Super(itemSize, normalized), _array(array)
  {
    Super::_data = _array.data();
    Super::_size = _array.size();
  }

  DefaultBufferAttribute(const DefaultBufferAttribute &source) :
     Super(source._itemSize, source._normalized, source._array, source._size)
  {}

  void resize(size_t newSize) {
    _array.resize(newSize);
    Super::_data = _array.data();
    Super::_size = newSize;
  }

  DefaultBufferAttribute(const std::vector<UV> &uvs, bool normalized)
     : Super(2, normalized)
  {
    resize(uvs.size() * Base::_itemSize);

    size_t offset = 0;
    for(const UV &uv : uvs) {
      _array[ offset ++ ] = uv.u();
      _array[ offset ++ ] = uv.v();
    }
  }

  DefaultBufferAttribute(const std::vector<Color> &colors, bool normalized)
     : Super(3, normalized)
  {
    resize(colors.size() * Base::_itemSize);

    size_t offset = 0;
    for(const Color &color : colors) {
      _array[ offset ++ ] = color.r;
      _array[ offset ++ ] = color.g;
      _array[ offset ++ ] = color.b;
    }
  }

  DefaultBufferAttribute(const std::vector<Index> &indices, bool normalized)
     : Super(3, normalized)
  {
    resize(indices.size() * Base::_itemSize);

    size_t offset = 0;
    for (size_t i = 0, l = _array.size(); i < l; i ++ ) {

      const Index &index = indices[ i ];

      _array[ offset ++ ] = index.a;
      _array[ offset ++ ] = index.b;
      _array[ offset ++ ] = index.c;
    }
  }

  DefaultBufferAttribute(std::vector<math::Vector2> vectors, bool normalized)
  : Super(2, normalized)
  {
    resize(vectors.size() * Base::_itemSize);

    size_t offset = 0;
    for(const math::Vector2 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
    }
  }

  DefaultBufferAttribute(std::vector<math::Vector3> vectors, bool normalized)
  : Super(3, normalized)
  {
    resize(vectors.size() * Base::_itemSize);

    size_t offset = 0;
    for(const math::Vector3 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
    }
  }

  DefaultBufferAttribute(std::vector<math::Vector4> vectors, bool normalized)
  : Super(4, normalized)
  {
    resize(vectors.size() * Base::_itemSize);

    size_t offset = 0;
    for(const math::Vector4 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
      _array[ offset ++ ] = vector.w();
    }
  }

public:
  using Ptr = std::shared_ptr<DefaultBufferAttribute>;

  static Ptr make(const std::vector<Type> &array, unsigned itemSize, bool normalized)
  {
    return Ptr(new DefaultBufferAttribute<Type>(array, itemSize, normalized));
  }

  static Ptr make(const std::vector<float> values, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(values, 1, normalized));
  }

  static Ptr make(const std::vector<UV> uvs, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(uvs, normalized));
  }

  static Ptr make(const std::vector<Color> colors, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(colors, normalized));
  }

  static Ptr make(const std::vector<Index> &indices, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(indices, normalized));
  }

  static Ptr make(const std::vector<math::Vector2> &vectors, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(vectors, normalized));
  }

  static Ptr make(const std::vector<math::Vector3> &vectors, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(vectors, normalized));
  }

  static Ptr make(const std::vector<math::Vector4> &vectors, bool normalized=false)
  {
    return Ptr(new DefaultBufferAttribute(vectors, normalized));
  }

  void set(const std::vector<Type> &array)
  {
    _array = array;
    Super::_data = _array.data();
    Super::_size = _array.size();
  }

  const std::vector<Type> &array() const {return _array;}
};

/**
 * a buffer attribute that wraps an externally managed buffer
 *
 * @tparam Type
 */
template <typename Type>
class ExternalBufferAttribute : public three::BufferAttributeT<Type>
{
  using Super = three::BufferAttributeT<Type>;

protected:
  ExternalBufferAttribute(Type *data, size_t itemSize, size_t itemCount)
     : three::BufferAttributeT<Type>(itemSize, true)
  {
    Super::_data = data;
    Super::_size = itemCount * itemSize;
  }

public:
  using Ptr = std::shared_ptr<ExternalBufferAttribute>;

  template <typename ItemT>
  static Ptr make(ItemT *data, size_t size) {
    return Ptr(new ExternalBufferAttribute(reinterpret_cast<Type *>(data), sizeof(ItemT) / sizeof(Type), size));
  }
};

}
#endif //THREE_QT_BUFFERATTRIBUTE_H
