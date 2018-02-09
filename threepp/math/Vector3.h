//
// Created by byter on 12.08.17.
//

#ifndef THREEPP_VECTOR3_H
#define THREEPP_VECTOR3_H

#include <cassert>
#include <algorithm>
#include <cmath>
#include <vector>
#include "Euler.h"
#include "Math.h"

namespace three {

using Vertex = math::Vector3;

class Camera;

template<typename T>
class BufferAttributeT;

namespace math {

Vector3 operator + (const Vector3 &left, const Vector3 &right);
Vector3 operator -(const Vector3 &left, const Vector3 &right);
Vector3 operator *(const Vector3 &left, const Vector3 &right);
Vector3 operator *(const Vector3 &vector, float scalar);
Vector3 operator *(const Vector3  &vector, const Matrix4 &matrix);
Vector3 operator / (const Vector3 &vector, float scalar);
Vector3 cross(const Vector3 &a, const Vector3 &b);

class Matrix4;
class Quaternion;
class Spherical;
class Cylindrical;

class Vector3
{
  union {
    struct {
      float _x, _y, _z;
    };
    float _elements[3];
  };

public:
  enum class Element {x=0, y=1, z=2};

  Vector3() : _x(0.0f), _y(0.0f), _z(0.0f) {}
  Vector3(float x, float y, float z) : _x(x), _y(y), _z(z) {}
  Vector3(float scalar) : _x(scalar), _y(scalar), _z(scalar) {}
  Vector3(const Vector3 &v) : _x(v._x), _y(v._y), _z(v._z) {}

  const float *elements() const {return _elements;}

  static Vector3 fromSpherical(const Spherical &s);

  static Vector3 fromCylindrical(const Cylindrical &c);

  static Vector3 fromMatrixPosition(const Matrix4 &m);

  static Vector3 fromBufferAttribute(const BufferAttributeT<float> &att, unsigned index);

  template <typename T>
  static Vector3 fromArray(const T *array, unsigned offset)
  {
    float x = array[ offset ];
    float y = array[ offset + 1 ];
    float z = array[ offset + 2 ];

    return Vector3(x, y, z);
  }

  static Vector3 fromMatrixColumn(const Matrix4 &m, unsigned index);

  static Vector3 fromMatrixScale(const Matrix4 &m);
  const float x() const {return _x;}
  const float y() const {return _y;}
  const float z() const {return _z;}

  Vector3 &set(float x, float y, float z) {
    _x = x; _y = y; _z = z;
    return *this;
  }

  float &x() {return _x;}
  float &y() {return _y;}
  float &z() {return _z;}

  float &operator[](Element element)
  {
    return _elements[(size_t)element];
  }

  const float operator[] (Element element) const
  {
    return _elements[(size_t)element];
  }

  Vector3 &operator +=(const Vector3 &vector)
  {
    _x += vector._x;
    _y += vector._y;
    _z += vector._z;

    return *this;
  }

  Vector3 &operator +=(float scalar)
  {
    _x += scalar;
    _y += scalar;
    _z += scalar;

    return *this;
  }

  Vector3 &operator -=(const Vector3 &v)
  {
    _x -= v._x;
    _y -= v._y;
    _z -= v._z;

    return *this;

  }

  Vector3 &operator -=(float scalar)
  {
    _x -= scalar;
    _y -= scalar;
    _z -= scalar;

    return *this;
  }

  Vector3 &operator *=(const Vector3 &v)
  {
    _x *= v._x;
    _y *= v._y;
    _z *= v._z;

    return *this;
  }

  Vector3 &operator *=(float scalar )
  {
    _x *= scalar;
    _y *= scalar;
    _z *= scalar;

    return *this;
  }

  //apply matrix3
  Vector3 &apply(const Matrix3 &m);

  //apply matrix4
  Vector3 &apply(const Matrix4 &m);

  //apply quaternion
  Vector3 &apply(const Quaternion &q);

  Vector3 &project(const Camera &camera);

  Vector3 &unproject(const Camera &camera);

  // input: THREE.Matrix4 affine matrix
  // vector interpreted as a direction
  Vector3 &transformDirection(const Matrix4 & m);

  Vector3 &operator /= (const Vector3 &v )
  {
    _x /= v._x;
    _y /= v._y;
    _z /= v._z;

    return *this;
  }

  Vector3 &operator /=(float scalar)
  {
    return *this *= ( 1.0f / scalar );
  }

  Vector3 &min(const Vector3 &v)
  {
    _x = std::min( _x, v._x );
    _y = std::min( _y, v._y );
    _z = std::min( _z, v._z );

    return *this;
  }

  Vector3 &max(const Vector3 &v)
  {
    _x = std::max( _x, v._x );
    _y = std::max( _y, v._y );
    _z = std::max( _z, v._z );

    return *this;
  }

  Vector3 &clamp(const Vector3 &min, const Vector3 &max)
  {
    // assumes min < max, componentwise
    _x = std::max( min._x, std::min( max._x, _x ) );
    _y = std::max( min._y, std::min( max._y, _y ) );
    _z = std::max( min._z, std::min( max._z, _z ) );

    return *this;
  }

  Vector3 &clamp(float minVal, float maxVal)
  {
    Vector3 min( minVal, minVal, minVal );
    Vector3 max( maxVal, maxVal, maxVal );

    return this->clamp(min, max);
  }

  Vector3 &clampLength(float min, float max)
  {
    float len = length();

    float div = (len > 0 ? len : 1) * std::max(min, std::min(max, len));
    return *this /= div;
  }

  Vector3 &floor()
  {
    _x = std::floor( _x );
    _y = std::floor( _y );
    _z = std::floor( _z );

    return *this;
  }

  Vector3 &ceil()
  {
    _x = std::ceil( _x );
    _y = std::ceil( _y );
    _z = std::ceil( _z );

    return *this;
  }

  Vector3 &round()
  {
    _x = ::round( _x );
    _y = ::round( _y );
    _z = ::round( _z );

    return *this;
  }

  Vector3 &roundToZero()
  {
    _x = _x < 0 ? std::ceil( _x ) : std::floor( _x );
    _y = _y < 0 ? std::ceil( _y ) : std::floor( _y );
    _z = _z < 0 ? std::ceil( _z ) : std::floor( _z );

    return *this;
  }

  Vector3 &negate()
  {
    _x = -_x;
    _y = -_y;
    _z = -_z;
    return *this;
  }

  float dot(const Vector3 &v) const
  {
    return _x * v._x + _y * v._y + _z * v._z;
  }

  float lengthSq() const
  {
    return _x * _x + _y * _y + _z * _z;
  }

  float length() const
  {
    return std::sqrt( _x * _x + _y * _y + _z * _z );
  }

  float manhattanLength() const
  {
    return std::abs(_x) + std::abs(_y) + std::abs(_z);
  }

  Vector3 &normalize()
  {
    float l = length();
    if(l) *this /= l;
    return *this;
  }

  Vector3 normalized() const
  {
    float len = length();
    return *this / (len > 0 ? len : 1.0f);
  }

  Vector3 &setLength(float length)
  {
    return normalize() *= length;
  }

  Vector3 &lerp(const Vector3 &v, float alpha)
  {
    _x += ( v._x - _x ) * alpha;
    _y += ( v._y - _y ) * alpha;
    _z += ( v._z - _z ) * alpha;

    return *this;
  }

  Vector3 &lerpVectors(const Vector3 &v1, const Vector3 &v2, float alpha)
  {
    *this = (v2 - v1) * alpha + v1;
    return *this;
  }

  Vector3 &cross(const Vector3 &v)
  {
    float x = _x, y = _y, z = _z;

    _x = y * v._z - z * v._y;
    _y = z * v._x - x * v._z;
    _z = x * v._y - y * v._x;

    return *this;
  }

  Vector3 &project(const Vector3 &vector)
  {
    float scalar = vector.dot(*this) / vector.lengthSq();
    *this = vector * scalar;

    return *this;
  }

  Vector3 &projectOnPlane(const Vector3 &planeNormal) 
  {
    Vector3 v1(*this);
    v1.project( planeNormal );

    return *this -= v1;
  }

  // reflect incident vector off plane orthogonal to normal
  // normal is assumed to have unit length
  Vector3 &reflect(const Vector3 &normal) 
  {
    Vector3 v1(normal);
    return *this -= (v1  * ( 2 * this->dot( normal ) ) );
  }

  float angleTo(const Vector3 &v ) const
  {
    float theta = dot( v ) / ( std::sqrt( lengthSq() * v.lengthSq() ) );

    // clamp, to handle numerical problems
    return std::acos( math::clamp( theta, -1.0f, 1.0f ) );
  }

  float distanceToSquared(const Vector3 &v) const
  {
     float dx = _x - v._x, dy = _y - v._y, dz = _z - v._z;

     return dx * dx + dy * dy + dz * dz;
  }
  
  float distanceTo(const Vector3 &v) const
  {
    return std::sqrt(distanceToSquared(v));
  }

  float manhattanDistance(const Vector3 &v) const 
  {
    return std::abs( _x - v._x ) + std::abs( _y - v._y ) + std::abs( _z - v._z );
  }

  template <typename Array>
  void toArray(Array array, unsigned offset=0) const
  {
    array[ offset ] = _x;
    array[ offset + 1 ] = _y;
    array[ offset + 2 ] = _z;
  }

  bool operator ==(const Vector3 &v) const
  {
    return ( ( v._x == _x ) && ( v._y == _y ) && ( v._z == _z ) );
  }
};

inline Vector3 operator + (const Vector3 &left, const Vector3 &right)
{
  Vector3 result {left};
  result += right;
  return result;
}

inline Vector3 operator -(const Vector3 &left, const Vector3 &right)
{
  Vector3 result {left};
  result -= right;
  return result;
}

inline Vector3 operator *(const Vector3 &left, const Vector3 &right)
{
  Vector3 result {left};
  result *= right;
  return result;
}

inline Vector3 operator *(const Vector3 &vector, float scalar)
{
  Vector3 result {vector};
  result *= scalar;
  return result;
}

inline Vector3 operator *(const Vector3  &vector, const Matrix4 &matrix)
{
  return Vector3(vector).apply(matrix);
}

inline Vector3 operator / (const Vector3&v1, const Vector3 &v2)
{
  Vector3 vector(v1);
  vector /= v2;

  return vector;
}

inline Vector3 operator / (const Vector3 &vector, float scalar)
{
  return vector * ( 1.0f / scalar );
}

inline Vector3 cross(const Vector3 &a, const Vector3 &b)
{
  float ax = a.x(), ay = a.y(), az = a.z();
  float bx = b.x(), by = b.y(), bz = b.z();

  return Vector3(ay * bz - az * by,
                 az * bx - ax * bz,
                 ax * by - ay * bx);
}

}
}
#endif //THREEPP_VECTOR3_H
