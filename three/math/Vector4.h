//
// Created by byter on 18.08.17.
//

#ifndef THREE_CPP_VECTOR4_H
#define THREE_CPP_VECTOR4_H

#include <cstring>
#include <cmath>
#include <algorithm>

namespace three {
namespace math {

class Matrix4;
class Quaternion;
class Euler;

class Vector4
{
  union {
    struct { float _x, _y, _z, _w; };
    float _elements[4];
  };

public:
  Vector4(float x, float y, float z, float w) : _x(x), _y(y), _z(z), _w(w) {}
  
  Vector4(const Vector4 &vector)
  {
    memcpy(_elements, vector._elements, sizeof(_elements));
  }

  Vector4(const float * array, unsigned offset)
  {
    memcpy(_elements, array, sizeof(_elements));
  }

  const float *elements() const {return _elements;}

  Vector4 &set(float x, float y, float z, float w )
  {
    _x = x;
    _y = y;
    _z = z;
    _w = w;

    return *this;
  }

  Vector4 &setScalar(float scalar)
  {
    _x = scalar;
    _y = scalar;
    _z = scalar;
    _w = scalar;

    return *this;
  }

  const float x() const {return _x;}
  const float y() const {return _y;}
  const float z() const {return _z;}
  const float w() const {return _w;}

  Vector4 &set(unsigned index, float value)
  {
    _elements[index] = value;
    return *this;
  }

  float get(unsigned index) const
  {
    return _elements[index];
  }

  float dot(const Vector4 &v ) const
  {
    return _x * v._x + _y * v._y + _z * v._z + _w * v._w;
  }

  float lengthSq() const
  {
    return _x * _x + _y * _y + _z * _z + _w * _w;
  }

  float length() const
  {
    return std::sqrt( _x * _x + _y * _y + _z * _z + _w * _w );
  }

  float manhattanLength() const
  {
    return std::abs( _x ) + std::abs( _y ) + std::abs( _z ) + std::abs( _w );
  }

  Vector4 &add(const Vector4 &v)
  {
    _x += v._x;
    _y += v._y;
    _z += v._z;
    _w += v._w;

    return *this;
  }

  Vector4 &addScalar(float s)
  {
    _x += s;
    _y += s;
    _z += s;
    _w += s;

    return *this;
  }

  Vector4 &addVectors(const Vector4 &a, const Vector4 &b)
  {
    _x = a._x + b._x;
    _y = a._y + b._y;
    _z = a._z + b._z;
    _w = a._w + b._w;

    return *this;
  }

  Vector4 &addScaledVector(const Vector4 &v, float s)
  {
    _x += v._x * s;
    _y += v._y * s;
    _z += v._z * s;
    _w += v._w * s;

    return *this;
  }

  Vector4 &sub(const Vector4 &v)
  {
    _x -= v._x;
    _y -= v._y;
    _z -= v._z;
    _w -= v._w;

    return *this;
  }

  Vector4 &sub(const Vector4 &a, const Vector4 &b)
  {
    _x = a._x - b._x;
    _y = a._y - b._y;
    _z = a._z - b._z;
    _w = a._w - b._w;

    return *this;
  }

  Vector4 &sub(float s)
  {
    _x -= s;
    _y -= s;
    _z -= s;
    _w -= s;

    return *this;
  }

  Vector4 &multiply(float scalar)
  {
    _x *= scalar;
    _y *= scalar;
    _z *= scalar;
    _w *= scalar;

    return *this;
  }

  Vector4 &divide(float scalar)
  {
    return multiply(1 / scalar);
  }

  //applyMatrix4
  Vector4 &operator *=(const Matrix4 &m);

  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
  // q is assumed to be normalized
  Vector4 &setAxisAngleFromQuaternion(const Quaternion &q);

  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm
  // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)
  Vector4 &setAxisAngleFromRotationMatrix(const Matrix4 &m);

  Vector4 &min(const Vector4 &v)
  {
    _x = std::min( _x, v._x );
    _y = std::min( _y, v._y );
    _z = std::min( _z, v._z );
    _w = std::min( _w, v._w );

    return *this;
  }

  Vector4 &max(const Vector4 &v)
  {
    _x = std::max( _x, v.x() );
    _y = std::max( _y, v.y() );
    _z = std::max( _z, v.z() );
    _w = std::max( _w, v.w() );

    return *this;
  }

  Vector4 &clamp(const Vector4 &min, const Vector4 &max)
  {
    // assumes min < max, componentwise

    _x = std::max( min._x, std::min( max._x, _x ) );
    _y = std::max( min._y, std::min( max._y, _y ) );
    _z = std::max( min._z, std::min( max._z, _z ) );
    _w = std::max( min._w, std::min( max._w, _w ) );

    return *this;
  }

  Vector4 &clampScalar(float minVal, float maxVal)
  {
    Vector4 min( minVal, minVal, minVal, minVal );
    Vector4 max( maxVal, maxVal, maxVal, maxVal );

    return clamp( min, max );
  }

  Vector4 &clampLength(float min, float max)
  {
    float len = length();

    return divide(len > 0 ? len : 1 ).multiply( std::max( min, std::min( max, len ) ) );
  }

  Vector4 &floor()
  {
    _x = std::floor( _x );
    _y = std::floor( _y );
    _z = std::floor( _z );
    _w = std::floor( _w );

    return *this;
  }

  Vector4 &ceil()
  {
    _x = std::ceil( _x );
    _y = std::ceil( _y );
    _z = std::ceil( _z );
    _w = std::ceil( _w );

    return *this;
  }

  Vector4 &round()
  {
    _x = std::round( _x );
    _y = std::round( _y );
    _z = std::round( _z );
    _w = std::round( _w );

    return *this;
  }

  Vector4 &roundToZero()
  {
    _x = ( _x < 0 ) ? std::ceil( _x ) : std::floor( _x );
    _y = ( _y < 0 ) ? std::ceil( _y ) : std::floor( _y );
    _z = ( _z < 0 ) ? std::ceil( _z ) : std::floor( _z );
    _w = ( _w < 0 ) ? std::ceil( _w ) : std::floor( _w );

    return *this;
  }

  Vector4 &negate()
  {
    _x = - _x;
    _y = - _y;
    _z = - _z;
    _w = - _w;

    return *this;
  }


  Vector4 &normalize()
  {
    float len = length();
    return divide(len > 0 ? len : 1 );
  }

  Vector4 &setLength(float length)
  {
    return normalize().multiply( length );
  }

  Vector4 &lerp(const Vector4 &v, float alpha)
  {
    _x += ( v._x - _x ) * alpha;
    _y += ( v._y - _y ) * alpha;
    _z += ( v._z - _z ) * alpha;
    _w += ( v._w - _w ) * alpha;

    return *this;
  }

  Vector4 &lerp(const Vector4 &v1, const Vector4 &v2, float alpha)
  {
    return sub( v2, v1 ).multiply( alpha ).add( v1 );
  }

  bool operator ==(const Vector4 & v)
  {
    return ( ( v._x == _x ) && ( v._y == _y ) && ( v._z == _z ) && ( v._w == _w ) );
  }

  void writeTo(float *array, unsigned offset=0)
  {
    memcpy(array+offset, _elements, sizeof(_elements));
  }
};

inline Vector4 operator - (const Vector4 &a, const Vector4 &b)
{
  float x = a.x() - b.x();
  float y = a.y() - b.y();
  float z = a.z() - b.z();
  float w = a.w() - b.w();

  return Vector4(x, y, z, w);
}

}
}


#endif //THREE_CPP_VECTOR4_H
