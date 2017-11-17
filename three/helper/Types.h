//
// Created by byter on 11/17/17.
//

#ifndef THREE_QT_TYPES_H
#define THREE_QT_TYPES_H

#include <string>
#include <vector>
#include <array>
#include "math/Vector3.h"

namespace three {

class Layers
{
  unsigned int mask;

public:

  void set(unsigned int channel) {
    mask = 1 << channel | 0;
  }

  void enable(unsigned int channel) {
    mask |= 1 << channel | 0;
  }

  void toggle(unsigned int channel) {
    mask ^= 1 << channel | 0;
  }

  void disable(unsigned int channel) {
    mask &= ~ ( 1 << channel | 0 );
  }

  bool test(const Layers &layers) const {
    return (mask & layers.mask ) != 0;
  }
};

struct Group {
  const uint32_t start;
  uint32_t count;
  const uint32_t materialIndex;

  Group(uint32_t start, uint32_t count, uint32_t materialIndex)
     : start(start), count(count), materialIndex(materialIndex) {}

  Group() : start(0), count(0), materialIndex(0) {}
};

class UV {
  union {
    struct { float _u, _v; };
    float _elements[2];
  };

public:

  UV(float u = 0, float v = 0) : _u( u ), _v( v ) { }
  UV( const UV& uv ) : _u(uv._u), _v(uv._v) {}

  UV& set( float uIn, float vIn )
  {
    _u = uIn;
    _v = vIn;

    return *this;
  }

  UV& lerpSelf( const UV& uv, float alpha )
  {
    _u += ( uv._u - _u ) * alpha;
    _v += ( uv._v - _v ) * alpha;

    return *this;
  }

  float &u() {return _u;}
  float &v() {return _v;}

  const float u() const {return _u;}
  const float v() const {return _v;}
};

using UV_Array = std::array<UV, 3>;

} // namespace three

#endif //THREE_QT_TYPES_H
