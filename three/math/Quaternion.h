//
// Created by byter on 12.08.17.
//

#ifndef THREE_QT_QUATERNION_H
#define THREE_QT_QUATERNION_H

#include <cassert>
#include <algorithm>
#include <cmath>
#include "helper/simplesignal.h"

namespace three {
namespace math {

class Euler;
class Vector3;
class Matrix4;

class Quaternion
{
  union {
    struct {
      float _x, _y, _z, _w;
    };
    float _elements[4];
  };

  Quaternion &set(const Quaternion &q, bool emitSignal) {
    _x = q._x; _y = q._y; _z = q._z; _w = q._w;
    if(emitSignal) onChange.emitSignal(*this);
  }

public:
  Quaternion(float x, float y, float z, float w) : _x(x), _y(y), _z(z), _w(w) {}

  Quaternion() : _x(0.0f), _y(0.0f), _z(0.0f), _w(1.0f) {}

  Quaternion(float scalar) : _x(scalar), _y(scalar), _z(scalar), _w(scalar) {}

  Quaternion(const Quaternion &q) : _x(q._x), _y(q._y), _z(q._z), _w(q._w) {}

  Quaternion &operator =(const Quaternion &q) {
    set(q, true);
  }

  Signal<void(const Quaternion &)> onChange;

  const float x() const {
    return _x;
  }

  const float y() const {
    return _y;
  }

  const float z() const {
    return _z;
  }

  const float w() const {
    return _w;
  }

  void slerpFlat(float *dst,
                 unsigned dstOffset,
                 float *src0,
                 unsigned srcOffset0,
                 float *src1,
                 unsigned srcOffset1,
                 float t) const
  {
    // fuzz-free, array-based Quaternion SLERP operation

    float x0 = src0[ srcOffset0 + 0 ],
       y0 = src0[ srcOffset0 + 1 ],
       z0 = src0[ srcOffset0 + 2 ],
       w0 = src0[ srcOffset0 + 3 ],

       x1 = src1[ srcOffset1 + 0 ],
       y1 = src1[ srcOffset1 + 1 ],
       z1 = src1[ srcOffset1 + 2 ],
       w1 = src1[ srcOffset1 + 3 ];

    if ( w0 != w1 || x0 != x1 || y0 != y1 || z0 != z1 ) {

      float s = 1 - t,

         cos = x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,

         dir = ( cos >= 0 ? 1 : - 1 ),
         sqrSin = 1 - cos * cos;

      // Skip the Slerp for tiny steps to avoid numeric problems:
      if ( sqrSin > std::numeric_limits<float>::epsilon()) {

        float sin = std::sqrt( sqrSin ),
           len = std::atan2( sin, cos * dir );

        s = std::sin( s * len ) / sin;
        t = std::sin( t * len ) / sin;

      }

      float tDir = t * dir;

      x0 = x0 * s + x1 * tDir;
      y0 = y0 * s + y1 * tDir;
      z0 = z0 * s + z1 * tDir;
      w0 = w0 * s + w1 * tDir;

      // Normalize in case we just did a lerp:
      if (s == 1 - t ) {
        float f = 1 / std::sqrt( x0 * x0 + y0 * y0 + z0 * z0 + w0 * w0 );

        x0 *= f;
        y0 *= f;
        z0 *= f;
        w0 *= f;
      }
    }

    dst[ dstOffset ] = x0;
    dst[ dstOffset + 1 ] = y0;
    dst[ dstOffset + 2 ] = z0;
    dst[ dstOffset + 3 ] = w0;
  }

  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
  // assumes axis is normalized
  Quaternion(const Vector3 &axis, float angle );

  Quaternion& set(const Vector3 &axis, float angle );

  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
  // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)
  explicit Quaternion(const Matrix4 &m);

  Quaternion& set(const Matrix4 &m, bool emitSignal=true);

  Quaternion &set(const Euler &euler, bool emitSignal=true);


  // assumes direction vectors vFrom and vTo are normalized
  Quaternion(const Vector3 &vFrom, const Vector3 &vTo);

  Quaternion &inverse()
  {
    return conjugate().normalize();
  }

  Quaternion &conjugate()
  {
    _x *= -1;
    _y *= -1;
    _z *= -1;

    onChange.emitSignal(*this);
    return *this;
  }

  float dot(const Quaternion &v) const
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

  Quaternion &normalize()
  {
    float l = length();
    if (l == 0) {
      _x = 0;
      _y = 0;
      _z = 0;
      _w = 1;
    }
    else {
      l = 1 / l;
      _x = _x * l;
      _y = _y * l;
      _z = _z * l;
      _w = _w * l;

    }
    return *this;
  }

  Quaternion &operator *=(const Quaternion &b)
  {
    float qax = _x, qay = _y, qaz = _z, qaw = _w;
    float qbx = b._x, qby = b._y, qbz = b._z, qbw = b._w;

    _x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    _y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    _z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    _w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    onChange.emitSignal(*this);
    return *this;
  }

  Quaternion& slerp( const Quaternion& qb, float t, bool emitSignal=true )
  {
    if(t == 0) return *this;
    if(t == 1) return set(qb, emitSignal);

    float x = _x, y = _y, z = _z, w = _w;

    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
    float cosHalfTheta = _w * qb._w + _x * qb._x + _y * qb._y + _z * qb._z;

    if ( cosHalfTheta < 0 ) {
      _w = -qb._w;
      _x = -qb._x;
      _y = -qb._y;
      _z = -qb._z;

      cosHalfTheta = -cosHalfTheta;
    }
    else {
      *this = qb;
    }

    if ( cosHalfTheta >= 1.0f ) {
      _w = w;
      _x = x;
      _y = y;
      _z = z;
      return *this;
    }

    float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);

    if (std::abs( sinHalfTheta ) < 0.001 ) {
      _w = 0.5f * ( w + _w );
      _x = 0.5f * ( x + _x );
      _y = 0.5f * ( y + _y );
      _z = 0.5f * ( z + _z );

      return *this;
    }

    float halfTheta = std::atan2( sinHalfTheta, cosHalfTheta );
    float ratioA = std::sin( ( 1 - t ) * halfTheta ) / sinHalfTheta,
       ratioB = std::sin( t * halfTheta ) / sinHalfTheta;

    _w = ( w * ratioA + _w * ratioB );
    _x = ( x * ratioA + _x * ratioB );
    _y = ( y * ratioA + _y * ratioB );
    _z = ( z * ratioA + _z * ratioB );

    onChange.emitSignal(*this);
    return *this;
  }

  bool operator ==(const Quaternion &quaternion) const
  {
    return quaternion._x == _x && quaternion._y == _y && quaternion._z == _z && quaternion._w == _w;
  }

  static Quaternion fromArray(const float *array, unsigned offset=0)
  {
    return Quaternion(array[ offset ],
                      array[ offset + 1 ],
                      array[ offset + 2 ],
                      array[ offset + 3 ]);
  }

  void writeTo(float *array, unsigned offset=0) const
  {
    array[ offset ] = _x;
    array[ offset + 1 ] = _y;
    array[ offset + 2 ] = _z;
    array[ offset + 3 ] = _w;
  }

};

inline Quaternion operator *(const Quaternion &q1, const Quaternion &q2) {
  Quaternion result(q1);
  result *= q2;
  return result;
}

}
}
#endif //THREE_QT_QUATERNION_H
