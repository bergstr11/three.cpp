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
#include <renderers/Resolver.h>

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

  BufferAttribute(bufferattribute::Resolver<void>::Ptr resolver=nullptr)
     : uuid(sole::uuid0()), resolver(resolver) {}

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
class BufferAttributeBase : public BufferAttribute
{
  std::vector<Type> _array;
  unsigned _itemSize;
  size_t _count;
  bool _normalized;

  bool _dynamic = false;
  UpdateRange _updateRange = {0, -1};

protected:
  BufferAttributeBase(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
     : _array(array), _itemSize(itemSize), _normalized(normalized), _count(array.size() / itemSize)
  {}

  explicit BufferAttributeBase(unsigned itemSize, bool normalized=false)
     : _array(std::vector<Type>()), _itemSize(itemSize), _normalized(normalized), _count(0)
  {}

  BufferAttributeBase(const BufferAttributeBase &source) :
     _array(source._array), _itemSize(source._itemSize), _count(source._count),
     _normalized(source._normalized), _dynamic(source._dynamic)
  {}

public:
  using Ptr = std::shared_ptr<BufferAttributeBase<Type>>;

  static Ptr make(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
  {
    return std::shared_ptr<BufferAttributeBase<Type>>(new BufferAttributeBase<Type>(array, itemSize, normalized));
  }

  Signal<void(const BufferAttributeBase<Type> &)> onUpload;

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

  BufferAttributeBase &setDynamic(bool value)
  {
    _dynamic = value;

    return *this;
  }


  BufferAttributeBase &copyAt(unsigned dstIndex, const BufferAttributeBase &srcAttribute, unsigned srcIndex)
  {
    std::memcpy(_array.data() + dstIndex * _itemSize,
                srcAttribute._array.data() + srcIndex * srcAttribute._itemSize,
                _itemSize);
    return *this;
  }

  BufferAttributeBase &copyArray(const std::vector<Type> &array)
  {
    _array = array;
    return *this;
  }

  BufferAttributeBase &copyColors(std::vector<Color> colors)
  {
    size_t offset = 0;

    for(const Color &color : colors) {
      _array[ offset ++ ] = color.r;
      _array[ offset ++ ] = color.g;
      _array[ offset ++ ] = color.b;
    }

    return *this;
  }

  BufferAttributeBase &copyIndices(const std::vector<Index> &indices)
  {
    unsigned offset = 0;

    for (size_t i = 0, l = _array.size(); i < l; i ++ ) {

      const Index &index = indices[ i ];

      _array[ offset ++ ] = index.a;
      _array[ offset ++ ] = index.b;
      _array[ offset ++ ] = index.c;

    }
    return *this;
  }

  BufferAttributeBase &copyVector2s(std::vector<math::Vector2> vectors )
  {
    size_t offset = 0;

    for(const math::Vector2 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
    }

    return *this;
  }

  BufferAttributeBase &copyVector3s(std::vector<math::Vector3> vectors)
  {
    size_t offset = 0;

    for(const math::Vector3 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
    }

    return *this;
  }

  BufferAttributeBase &copyVector4s(std::vector<math::Vector4> vectors)
  {
    size_t offset = 0;

    for(const math::Vector4 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
      _array[ offset ++ ] = vector.w();
    }

    return *this;
  }

  BufferAttributeBase &set(Type value, size_t offset=0)
  {
    _array[offset] = value;

    return *this;
  }

  Type get_x(size_t index) const
  {
    return _array[ index * _itemSize ];
  }

  BufferAttributeBase &set_x(size_t index, Type x)
  {
    _array[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index) const
  {
    return _array[ index * _itemSize + 1 ];
  }

  BufferAttributeBase &set_y(size_t index, Type y)
  {
    _array[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type get_z(size_t index) const
  {
    return _array[ index * _itemSize + 2 ];
  }

  BufferAttributeBase &set_z(size_t index, Type z)
  {
    _array[ index * _itemSize + 2 ] = z;

    return *this;
  }

  Type get_w(size_t index)
  {
    return _array[ index * _itemSize + 3 ];
  }

  BufferAttributeBase &set_w(size_t index, Type w)
  {
    _array[ index * _itemSize + 3 ] = w;

    return *this;
  }

  BufferAttributeBase &setXY(size_t index, Type x, Type y)
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;

    return *this;
  }

  BufferAttributeBase &setXYZ(size_t index, Type x, Type y, Type z )
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;
    _array[ index + 2 ] = z;

    return *this;
  }

  BufferAttributeBase &setXYZW(size_t index, Type x, Type y, Type z, Type w)
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
