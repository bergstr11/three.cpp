//
// Created by byter on 23.08.17.
//

#ifndef THREE_QT_BUFFERATTRIBUTE_H
#define THREE_QT_BUFFERATTRIBUTE_H

#include <vector>
#include <cstring>
#include <Color.h>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

namespace three {

class BufferAttributeBase
{
public:
  using Ptr = std::shared_ptr<BufferAttributeBase>;

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
  static Ptr make(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
  {
    return std::make_shared<BufferAttribute<Type>>(array, itemSize, normalized);
  }

  BufferAttribute(const std::vector<Type> &array, unsigned itemSize, bool normalized=false)
    : _array(array), _itemSize(itemSize), _normalized(normalized), _count(array.size() / itemSize)
  {}
  BufferAttribute(unsigned itemSize, bool normalized=false)
     : _array(std::vector<Type>()), _itemSize(itemSize), _normalized(normalized), _count(0)
  {}
  BufferAttribute(const BufferAttribute&source) :
     _array(source._array), _itemSize(source._itemSize), _count(source._count),
     _normalized(source._normalized), _dynamic(source._dynamic)
  {}

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

#if 0
  BufferAttribute &copyIndices( indices ) {

    var array = _array, offset = 0;

    for ( var i = 0, l = indices.length; i < l; i ++ ) {

      var index = indices[ i ];

      array[ offset ++ ] = index.a;
      array[ offset ++ ] = index.b;
      array[ offset ++ ] = index.c;

    }

    return *this;

  }
#endif

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

  Type get_x(size_t index) 
  {
    return _array[ index * _itemSize ];
  }

  BufferAttribute &set_x(size_t index, Type x) 
  {
    _array[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index)
  {
    return _array[ index * _itemSize + 1 ];
  }

  BufferAttribute &set_y(size_t index, Type y)
  {
    _array[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type get_z(size_t index)
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
};

}
#endif //THREE_QT_BUFFERATTRIBUTE_H
