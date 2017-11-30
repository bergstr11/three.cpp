//
// Created by byter on 21.10.17.
//

#ifndef THREE_QT_INTERLEAVEDBUFFERATTRIBUTE_H
#define THREE_QT_INTERLEAVEDBUFFERATTRIBUTE_H

#include "BufferAttribute.h"
#include "InterleavedBuffer.h"

namespace three {

class InterleavedBufferAttribute : public BufferAttribute
{
  InterleavedBuffer &_data;
  size_t _offset;

public:
  InterleavedBufferAttribute(InterleavedBuffer &buffer, unsigned itemSize, unsigned offset, bool normalized=true)
     : BufferAttribute(bufferattribute::ResolverT<InterleavedBufferAttribute>::make(*this), itemSize, normalized), _data(buffer)
  {}

  size_t count() const override {return _data.count();}

  size_t offset() const {return _offset;}

  const InterleavedBuffer &data() const {return _data;}

  const std::vector<float> &array() const {return _data.array();}

  InterleavedBufferAttribute &setX(unsigned index, float x) 
  {
    _data._array[ index * _data._stride + _offset ] = x;

    return *this;
  }

  InterleavedBufferAttribute &setY(unsigned index, float y)
  {
    _data._array[ index * _data._stride + _offset + 1 ] = y;

    return *this;
  }

  InterleavedBufferAttribute &setZ(unsigned index, float z)
  {
    _data._array[ index * _data._stride + _offset + 2 ] = z;

    return *this;
  }

  InterleavedBufferAttribute &setW(unsigned index, float w)
  {
    _data._array[ index * _data._stride + _offset + 3 ] = w;

    return *this;
  }

  float getX(unsigned index)
  {
    return _data._array[ index * _data._stride + _offset ];
  }

  float getY(unsigned index)
  {
    return _data._array[ index * _data._stride + _offset + 1 ];
  }

  float getZ(unsigned index)
  {
    return _data._array[ index * _data._stride + _offset + 2 ];
  }

  float getW(unsigned index)
  {
    return _data._array[ index * _data._stride + _offset + 3 ];
  }

  InterleavedBufferAttribute &setXY(unsigned index, float x, float y)
  {
    index = index * _data._stride + _offset;

    _data._array[ index + 0 ] = x;
    _data._array[ index + 1 ] = y;

    return *this;
  }

  InterleavedBufferAttribute &setXYZ(unsigned index, float x, float y, float z)
  {
    index = index * _data._stride + _offset;

    _data._array[ index + 0 ] = x;
    _data._array[ index + 1 ] = y;
    _data._array[ index + 2 ] = z;

    return *this;
  }

  InterleavedBufferAttribute &setXYZW(unsigned index, float x, float y, float z, float w)
  {
    index = index * _data._stride + _offset;

    _data._array[ index + 0 ] = x;
    _data._array[ index + 1 ] = y;
    _data._array[ index + 2 ] = z;
    _data._array[ index + 3 ] = w;

    return *this;
  }
};

}
#endif //THREE_QT_INTERLEAVEDBUFFERATTRIBUTE_H
