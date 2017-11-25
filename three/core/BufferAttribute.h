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

class BufferAttribute
{
protected:
  unsigned _version = 0;

  explicit BufferAttribute(bufferattribute::Resolver::Ptr resolver)
     : uuid(sole::uuid0()), resolver(resolver) {}

  BufferAttribute()
     : uuid(sole::uuid0()), resolver(bufferattribute::Resolver::makeNull()) {}

public:
  using Ptr = std::shared_ptr<BufferAttribute>;

  const bufferattribute::Resolver::Ptr resolver;

  const sole::uuid uuid;

  void needsUpdate() {_version++;}
  bool needsUpdate() const {return _version > 0;}

  unsigned version() const {return _version;}

  virtual void apply(const math::Matrix4 &matrix) = 0;

  virtual void apply(const math::Matrix3 &matrix) = 0;
};

template <typename Type>
class BufferAttributeT : public BufferAttribute
{
  std::vector<Type> _array;
  unsigned _itemSize;
  size_t _count;
  bool _normalized;

  bool _dynamic = false;
  UpdateRange _updateRange = {0, -1};

protected:
  BufferAttributeT(const std::vector<Type> &array, unsigned itemSize, bool normalized)
     : _array(array), _itemSize(itemSize), _normalized(normalized), _count(array.size() / itemSize)
  {}

  BufferAttributeT(unsigned itemSize, bool normalized)
     : _array(std::vector<Type>()), _itemSize(itemSize), _normalized(normalized), _count(0)
  {}

  BufferAttributeT(const BufferAttributeT &source) :
     _array(source._array), _itemSize(source._itemSize), _count(source._count),
     _normalized(source._normalized), _dynamic(source._dynamic)
  {}

  BufferAttributeT(const std::vector<float> &values, bool normalized)
     : BufferAttributeT(values, 1, normalized)
  {
  }

  BufferAttributeT(const std::vector<UV> &uvs, bool normalized)
     : BufferAttributeT(2, normalized)
  {
    _array.resize(uvs.size() * _itemSize);

    size_t offset = 0;
    for(const UV &uv : uvs) {
      _array[ offset ++ ] = uv.u();
      _array[ offset ++ ] = uv.v();
    }
  }

  BufferAttributeT(const std::vector<Color> &colors, bool normalized)
     : BufferAttributeT(3, normalized)
  {
    _array.resize(colors.size() * _itemSize);

    size_t offset = 0;
    for(const Color &color : colors) {
      _array[ offset ++ ] = color.r;
      _array[ offset ++ ] = color.g;
      _array[ offset ++ ] = color.b;
    }
  }

  BufferAttributeT(const std::vector<Index> &indices, bool normalized)
     : BufferAttributeT(3, normalized)
  {
    _array.resize(indices.size() * _itemSize);

    size_t offset = 0;
    for (size_t i = 0, l = _array.size(); i < l; i ++ ) {

      const Index &index = indices[ i ];

      _array[ offset ++ ] = index.a;
      _array[ offset ++ ] = index.b;
      _array[ offset ++ ] = index.c;

    }
  }

  BufferAttributeT(std::vector<math::Vector2> vectors, bool normalized)
     : BufferAttributeT(2, normalized)
  {
    _array.resize(vectors.size() * _itemSize);

    size_t offset = 0;
    for(const math::Vector2 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
    }
  }

  BufferAttributeT(std::vector<math::Vector3> vectors, bool normalized)
     : BufferAttributeT(3, normalized)
  {
    _array.resize(vectors.size() * _itemSize);

    size_t offset = 0;
    for(const math::Vector3 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
    }
  }

  BufferAttributeT(std::vector<math::Vector4> vectors, bool normalized)
     : BufferAttributeT(4, normalized)
  {
    _array.resize(vectors.size() * _itemSize);

    size_t offset = 0;
    for(const math::Vector4 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
      _array[ offset ++ ] = vector.w();
    }
  }

public:
  using Ptr = std::shared_ptr<BufferAttributeT<Type>>;

  static Ptr make(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
  {
    return std::shared_ptr<BufferAttributeT<Type>>(new BufferAttributeT<Type>(array, itemSize, normalized));
  }

  static Ptr make(const std::vector<float> values, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(values, normalized));
  }

  static Ptr make(const std::vector<UV> uvs, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(uvs, normalized));
  }

  static Ptr make(const std::vector<Color> colors, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(colors, normalized));
  }

  static Ptr make(const std::vector<Index> &indices, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(indices, normalized));
  }

  static Ptr make(const std::vector<math::Vector2> &vectors, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(vectors, normalized));
  }

  static Ptr make(const std::vector<math::Vector3> &vectors, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(vectors, normalized));
  }

  static Ptr make(const std::vector<math::Vector4> &vectors, bool normalized=false)
  {
    return Ptr(new BufferAttributeT(vectors, normalized));
  }

  Signal<void(const BufferAttributeT<Type> &)> onUpload;

  const size_t size() const {return _array.size();}

  const size_t count() const {return _count;}

  const std::vector<Type> &array() const {return _array;}

  const Type operator [] (size_t index) {return _array[index];}

  void set(const std::vector<Type> &array)
  {
    _count = array.size() / _itemSize;
    _array = array;
  }

  GLenum glType() const {return Cpp2GL<Type>::glEnum;}

  unsigned bytesPerElement() const {return sizeof(Type);}

  bool dynamic() const {return _dynamic;}

  const Type *data() const {return _array.data();}

  const Type *data(uint32_t offset) const {return _array.data()+offset;}

  UpdateRange &updateRange() {return _updateRange;}

  BufferAttributeT &setDynamic(bool value)
  {
    _dynamic = value;

    return *this;
  }


  BufferAttributeT &copyAt(unsigned dstIndex, const BufferAttributeT &srcAttribute, unsigned srcIndex)
  {
    std::memcpy(_array.data() + dstIndex * _itemSize,
                srcAttribute._array.data() + srcIndex * srcAttribute._itemSize,
                _itemSize);
    return *this;
  }

  BufferAttributeT &copyArray(const std::vector<Type> &array)
  {
    _array = array;
    return *this;
  }

  BufferAttributeT &set(Type value, size_t offset=0)
  {
    _array[offset] = value;

    return *this;
  }

  Type get_x(size_t index) const
  {
    return _array[ index * _itemSize ];
  }

  BufferAttributeT &set_x(size_t index, Type x)
  {
    _array[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index) const
  {
    return _array[ index * _itemSize + 1 ];
  }

  BufferAttributeT &set_y(size_t index, Type y)
  {
    _array[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type get_z(size_t index) const
  {
    return _array[ index * _itemSize + 2 ];
  }

  BufferAttributeT &set_z(size_t index, Type z)
  {
    _array[ index * _itemSize + 2 ] = z;

    return *this;
  }

  Type get_w(size_t index)
  {
    return _array[ index * _itemSize + 3 ];
  }

  BufferAttributeT &set_w(size_t index, Type w)
  {
    _array[ index * _itemSize + 3 ] = w;

    return *this;
  }

  BufferAttributeT &setXY(size_t index, Type x, Type y)
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;

    return *this;
  }

  BufferAttributeT &setXYZ(size_t index, Type x, Type y, Type z )
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;
    _array[ index + 2 ] = z;

    return *this;
  }

  BufferAttributeT &setXYZW(size_t index, Type x, Type y, Type z, Type w)
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;
    _array[ index + 2 ] = z;
    _array[ index + 3 ] = w;

    return *this;
  }

  math::Box3 box3()
  {
    Type minX = std::numeric_limits<Type>::infinity();
    Type minY = std::numeric_limits<Type>::infinity();
    Type minZ = std::numeric_limits<Type>::infinity();

    Type maxX = - std::numeric_limits<Type>::infinity();
    Type maxY = - std::numeric_limits<Type>::infinity();
    Type maxZ = - std::numeric_limits<Type>::infinity();

    for(size_t i = 0; i < _count; i++) {
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

  void apply(const math::Matrix4 &matrix) override
  {
    for(size_t i = 0; i < _count; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }

  void apply(const math::Matrix3 &matrix) override
  {
    for(size_t i = 0; i < _count; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }
};

}
#endif //THREE_QT_BUFFERATTRIBUTE_H
