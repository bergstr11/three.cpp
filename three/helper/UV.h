//
// Created by byter on 20.08.17.
//

#ifndef THREE_QT_UV_H
#define THREE_QT_UV_H

namespace three {

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

} // namespace three

#endif //THREE_QT_UV_H
