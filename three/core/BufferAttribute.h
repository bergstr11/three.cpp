//
// Created by byter on 23.08.17.
//

#ifndef THREE_QT_BUFFERATTRIBUTE_H
#define THREE_QT_BUFFERATTRIBUTE_H

#include <vector>
#include <cstring>
#include <Color.h>
#include <memory>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Box3.h"

namespace three {

struct Index
{
  const uint32_t a, b, c;

  Index(uint32_t a, uint32_t b, uint32_t c) : a(a), b(b), c(c) {}
};

class BufferAttributeBase
{
public:
  bool needsUpdate = false;

  virtual void apply(const math::Matrix4 &matrix) = 0;

  virtual void apply(const math::Matrix3 &matrix) = 0;
};

template <typename Type>
class BufferAttribute : public BufferAttributeBase
{
  std::vector<Type> _array;
  unsigned _itemSize;
  size_t _count;
  bool _normalized;

  bool _dynamic = false;
  unsigned _updateRange = 0;
  unsigned _updateCount = -1;

public:
  using Ptr = std::shared_ptr<BufferAttribute<Type>>;

  static Ptr make(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
  {
    return std::make_shared<BufferAttribute<Type>>(array, itemSize, normalized);
  }

  BufferAttribute(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
    : _array(array), _itemSize(itemSize), _normalized(normalized), _count(array.size() / itemSize)
  {}

  explicit BufferAttribute(unsigned itemSize, bool normalized=false)
     : _array(std::vector<Type>()), _itemSize(itemSize), _normalized(normalized), _count(0)
  {}

  BufferAttribute(const BufferAttribute&source) :
     _array(source._array), _itemSize(source._itemSize), _count(source._count),
     _normalized(source._normalized), _dynamic(source._dynamic)
  {}

  const size_t size() const {return _array.size();}

  const size_t count() const {return _count;}

  const std::vector<Type> &array() const {return _array;}

  const Type operator [] (size_t index) {return _array[index];}

  void set(const std::vector<Type> &array)
  {
    _count = array.size() / _itemSize;
    _array = array;
  }

  BufferAttribute &setDynamic(bool value)
  {
    _dynamic = value;

    return *this;
  }


  BufferAttribute &copyAt(unsigned dstIndex, const BufferAttribute &srcAttribute, unsigned srcIndex)
  {
    std::memcpy(_array.data() + dstIndex * _itemSize,
                srcAttribute._array.data() + srcIndex * srcAttribute._itemSize,
                _itemSize);
    return *this;
  }

  BufferAttribute &copyArray(const std::vector<Type> &array)
  {
    _array = array;
    return *this;
  }

  BufferAttribute &copyColors(std::vector<Color> colors)
  {
    size_t offset = 0;

    for(const Color &color : colors) {
      _array[ offset ++ ] = color.r;
      _array[ offset ++ ] = color.g;
      _array[ offset ++ ] = color.b;
    }

    return *this;
  }

  BufferAttribute &copyIndices(const std::vector<Index> &indices)
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

  BufferAttribute &copyVector2s(std::vector<math::Vector2> vectors )
  {
    size_t offset = 0;

    for(const math::Vector2 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
    }

    return *this;
  }

  BufferAttribute &copyVector3s(std::vector<math::Vector3> vectors)
  {
    size_t offset = 0;

    for(const math::Vector3 &vector : vectors) {
      _array[ offset ++ ] = vector.x();
      _array[ offset ++ ] = vector.y();
      _array[ offset ++ ] = vector.z();
    }

    return *this;
  }

  BufferAttribute &copyVector4s(std::vector<math::Vector4> vectors)
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

  BufferAttribute &set(Type value, size_t offset=0) 
  {
    _array[offset] = value;

    return *this;
  }

  Type get_x(size_t index) const
  {
    return _array[ index * _itemSize ];
  }

  BufferAttribute &set_x(size_t index, Type x) 
  {
    _array[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index) const
  {
    return _array[ index * _itemSize + 1 ];
  }

  BufferAttribute &set_y(size_t index, Type y)
  {
    _array[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type get_z(size_t index) const
  {
    return _array[ index * _itemSize + 2 ];
  }

  BufferAttribute &set_z(size_t index, Type z)
  {
    _array[ index * _itemSize + 2 ] = z;

    return *this;
  }

  Type get_w(size_t index)
  {
    return _array[ index * _itemSize + 3 ];
  }

  BufferAttribute &set_w(size_t index, Type w)
  {
    _array[ index * _itemSize + 3 ] = w;

    return *this;
  }

  BufferAttribute &setXY(size_t index, Type x, Type y)
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;

    return *this;
  }

  BufferAttribute &setXYZ(size_t index, Type x, Type y, Type z )
  {
    index *= _itemSize;

    _array[ index + 0 ] = x;
    _array[ index + 1 ] = y;
    _array[ index + 2 ] = z;

    return *this;
  }

  BufferAttribute &setXYZW(size_t index, Type x, Type y, Type z, Type w)
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
