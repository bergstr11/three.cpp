//
// Created by byter on 21.10.17.
//

#ifndef THREEPP_INTERLEAVEDBUFFERATTRIBUTE_H
#define THREEPP_INTERLEAVEDBUFFERATTRIBUTE_H

#include "BufferAttribute.h"
#include "InterleavedBuffer.h"

namespace three {

class InterleavedBufferAttribute : public BufferAttribute
{
  InterleavedBuffer &_buffer;
  size_t _offset;

public:
  InterleavedBufferAttribute(InterleavedBuffer &buffer, unsigned itemSize, unsigned offset, bool normalized=true)
     : BufferAttribute(bufferattribute::ResolverT<InterleavedBufferAttribute>::make(*this), itemSize, normalized), _buffer(buffer)
  {}

  size_t count() const {return _buffer.count();}

  size_t offset() const {return _offset;}

  const InterleavedBuffer &buffer() const {return _buffer;}

  const std::vector<float> &array() const {return _buffer.array();}

  InterleavedBufferAttribute &setX(unsigned index, float x) 
  {
    _buffer._array[ index * _buffer._stride + _offset ] = x;

    return *this;
  }

  InterleavedBufferAttribute &setY(unsigned index, float y)
  {
    _buffer._array[ index * _buffer._stride + _offset + 1 ] = y;

    return *this;
  }

  InterleavedBufferAttribute &setZ(unsigned index, float z)
  {
    _buffer._array[ index * _buffer._stride + _offset + 2 ] = z;

    return *this;
  }

  InterleavedBufferAttribute &setW(unsigned index, float w)
  {
    _buffer._array[ index * _buffer._stride + _offset + 3 ] = w;

    return *this;
  }

  float getX(unsigned index)
  {
    return _buffer._array[ index * _buffer._stride + _offset ];
  }

  float getY(unsigned index)
  {
    return _buffer._array[ index * _buffer._stride + _offset + 1 ];
  }

  float getZ(unsigned index)
  {
    return _buffer._array[ index * _buffer._stride + _offset + 2 ];
  }

  float getW(unsigned index)
  {
    return _buffer._array[ index * _buffer._stride + _offset + 3 ];
  }

  InterleavedBufferAttribute &setXY(unsigned index, float x, float y)
  {
    index = index * _buffer._stride + _offset;

    _buffer._array[ index + 0 ] = x;
    _buffer._array[ index + 1 ] = y;

    return *this;
  }

  InterleavedBufferAttribute &setXYZ(unsigned index, float x, float y, float z)
  {
    index = index * _buffer._stride + _offset;

    _buffer._array[ index + 0 ] = x;
    _buffer._array[ index + 1 ] = y;
    _buffer._array[ index + 2 ] = z;

    return *this;
  }

  InterleavedBufferAttribute &setXYZW(unsigned index, float x, float y, float z, float w)
  {
    index = index * _buffer._stride + _offset;

    _buffer._array[ index + 0 ] = x;
    _buffer._array[ index + 1 ] = y;
    _buffer._array[ index + 2 ] = z;
    _buffer._array[ index + 3 ] = w;

    return *this;
  }

  const void *data(size_t offset) const override
  {
    return _buffer.array().data() + offset;
  }

  GLenum glType() const override
  {
    return Cpp2GL<float>::glEnum;
  }

  size_t byteCount() const override
  {
    return _buffer.array().size() * sizeof(float);
  }

  unsigned int bytesPerElement() const override
  {
    return sizeof(float);
  }
};

}
#endif //THREEPP_INTERLEAVEDBUFFERATTRIBUTE_H
