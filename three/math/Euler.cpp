//
// Created by byter on 29.07.17.
//

#include "math/Euler.h"
#include "Math.h"

#include <cmath>

namespace three {
namespace math {

Euler & Euler::operator = (const Euler &euler)
{
  _x = euler._x;
  _y = euler._y;
  _z = euler._z;
  _order = euler._order;

  onChange.emitSignal(*this);
}

void Euler::set(const Matrix4 &m, RotationOrder order, bool emitSignal)
{
  // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

  const float *te = m.elements();
  float m11 = te[ 0 ], m12 = te[ 4 ], m13 = te[ 8 ];
  float m21 = te[ 1 ], m22 = te[ 5 ], m23 = te[ 9 ];
  float m31 = te[ 2 ], m32 = te[ 6 ], m33 = te[ 10 ];

  if(order == RotationOrder::Default) order = _order;

  switch(order) {
    case XYZ: {
      _y = (float) asin(clamp(m13, -1.0f, 1.0f));

      if (std::abs(m13) < 0.99999) {
        _x = (float) atan2(-m23, m33);
        _z = (float) atan2(-m12, m11);
      }
      else {
        _x = (float) atan2(m32, m22);
        _z = 0;
      }
      break;
    }
    case YXZ: {
      _x = (float)asin( - clamp( m23, -1.0f, 1.0f ) );

      if ( std::abs( m23 ) < 0.99999 ) {
        _y = (float)atan2( m13, m33 );
        _z = (float)atan2( m21, m22 );
      }
      else {
        _y = (float)atan2( - m31, m11 );
        _z = 0;
      }
      break;
    }
    case ZXY: {
      _x = (float)asin( clamp( m32, -1.0f, 1.0f ) );

      if ( std::abs( m32 ) < 0.99999 ) {
        _y = (float)atan2( - m31, m33 );
        _z = (float)atan2( - m12, m22 );
      }
      else {
        _y = 0;
        _z = (float)atan2( m21, m11 );
      }
      break;
    }
    case ZYX: {
      _y = (float)asin( - clamp( m31, -1.0f, 1.0f ) );

      if ( std::abs( m31 ) < 0.99999 ) {
        _x = (float)atan2( m32, m33 );
        _z = (float)atan2( m21, m11 );
      }
      else {
        _x = 0;
        _z = (float)atan2( - m12, m22 );
      }
      break;
    }
    case YZX: {
      _z = std::asin( clamp(m21, -1.0f, 1.0f ) );

      if ( std::abs( m21 ) < 0.99999 ) {
        _x = (float)atan2( - m23, m22 );
        _y = (float)atan2( - m31, m11 );
      }
      else {
        _x = 0;
        _y = (float)atan2( m13, m33 );
      }
      break;
    }
    case XZY: {
      _z = (float)asin( - clamp(m12, -1.0f, 1.0f) );

      if ( std::abs( m12 ) < 0.99999 ) {
        _x = (float)atan2( m32, m22 );
        _y = (float)atan2( m13, m11 );
      }
      else {
        _x = (float)atan2(-m23, m33 );
        _y = 0;
      }
      break;
    }
  }
  _order = order;

  if(emitSignal) onChange.emitSignal(*this);
}

void Euler::set(float x, float y, float z, RotationOrder order, bool emitSignal)
{
  _x = x;
  _y = y;
  _z = z;
  _order = order;

  if(emitSignal) onChange.emitSignal(*this);
}

void Euler::set(const Quaternion &q, RotationOrder order, bool emitSignal)
{
  Matrix4 matrix = Matrix4::rotation( q );

  set(matrix, order, emitSignal);
}

void Euler::set(const Vector3 &v, RotationOrder order, bool emitSignal)
{
  set( v.x(), v.y(), v.z(), order, emitSignal);
}

Quaternion Euler::toQuaternion() const
{
  // http://www.mathworks.com/matlabcentral/fileexchange/
  // 	20696-function-to-convert-between-dcm-euler-angles-quaternions-and-euler-vectors/
  //	content/SpinCalc.m

  auto c1 = cos( _x / 2 );
  auto c2 = cos( _y / 2 );
  auto c3 = cos( _z / 2 );

  auto s1 = sin( _x / 2 );
  auto s2 = sin( _y / 2 );
  auto s3 = sin( _z / 2 );

  switch(_order) {
    case XYZ: {
      auto x = s1 * c2 * c3 + c1 * s2 * s3;
      auto y = c1 * s2 * c3 - s1 * c2 * s3;
      auto z = c1 * c2 * s3 + s1 * s2 * c3;
      auto w = c1 * c2 * c3 - s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
    case YXZ: {
      auto x = s1 * c2 * c3 + c1 * s2 * s3;
      auto y = c1 * s2 * c3 - s1 * c2 * s3;
      auto z = c1 * c2 * s3 - s1 * s2 * c3;
      auto w = c1 * c2 * c3 + s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
    case ZXY: {
      auto x = s1 * c2 * c3 - c1 * s2 * s3;
      auto y = c1 * s2 * c3 + s1 * c2 * s3;
      auto z = c1 * c2 * s3 + s1 * s2 * c3;
      auto w = c1 * c2 * c3 - s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
    case ZYX: {
      auto x = s1 * c2 * c3 - c1 * s2 * s3;
      auto y = c1 * s2 * c3 + s1 * c2 * s3;
      auto z = c1 * c2 * s3 - s1 * s2 * c3;
      auto w = c1 * c2 * c3 + s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
    case YZX: {
      auto x = s1 * c2 * c3 + c1 * s2 * s3;
      auto y = c1 * s2 * c3 + s1 * c2 * s3;
      auto z = c1 * c2 * s3 - s1 * s2 * c3;
      auto w = c1 * c2 * c3 - s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
    case XZY: {
      auto x = s1 * c2 * c3 - c1 * s2 * s3;
      auto y = c1 * s2 * c3 - s1 * c2 * s3;
      auto z = c1 * c2 * s3 + s1 * s2 * c3;
      auto w = c1 * c2 * c3 + s1 * s2 * s3;
      return {(float)w, (float)x, (float)y, (float)z};
    }
  }
}

void Euler::reorder(RotationOrder order)
{
  set(toQuaternion(), order);
}

bool Euler::operator ==(const Euler &euler)
{
  return ( euler._x == _x ) && ( euler._y == _y ) && ( euler._z == _z ) && ( euler._order == _order );
}

}
}