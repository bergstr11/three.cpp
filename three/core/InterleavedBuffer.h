//
// Created by byter on 21.10.17.
//

#ifndef THREE_QT_INTERLEAVEDBUFFER_H
#define THREE_QT_INTERLEAVEDBUFFER_H

#include <vector>
#include <three/helper/simplesignal.h>

namespace three {

class InterleavedBuffer
{
  friend class InterleavedBufferAttribute;

  std::vector<float> _array;
  size_t _stride, _count;
  bool _dynamic = false;
  unsigned _version = 0;

  UpdateRange _updateRange;

public:
  InterleavedBuffer(const std::vector<float> &array, size_t stride)
     : _array(array), _stride(stride), _count(array.size() / stride) {}

  Signal<void()> onUpload;

  bool invalidate() {_version++;}

  size_t stride() const {return _stride;}
  size_t count() const {return _count;}
  const std::vector<float> &array() const {return _array;}

  InterleavedBuffer &setArray(const std::vector<float> &array)
  {
    _count = (unsigned)(array.size() / _stride);
    _array = array;
    return *this;
  }

  InterleavedBuffer &setDynamic(bool value)
  {
    _dynamic = value;
    return *this;
  }

  InterleavedBuffer &copyAt(unsigned index1, const InterleavedBuffer &attribute, unsigned index2)
  {
    index1 *= _stride;
    index2 *= attribute._stride;

    if(_array.size() < index1 + _stride) {
      _array.resize(index1 + _stride);
    }
    for (unsigned i = 0; i < _stride; i ++ ) {

      _array[ index1 + i ] = attribute._array[ index2 + i ];
    }
    return *this;
  }

  InterleavedBuffer &set(float value, unsigned offset=0)
  {
    _array[offset] = value;

    return *this;
  }
};

}
#endif //THREE_QT_INTERLEAVEDBUFFER_H
