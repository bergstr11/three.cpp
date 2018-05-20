//
// Created by byter on 18.08.17.
//

#include "Quaternion.h"
#include "Euler.h"
#include "Vector3.h"
#include "Matrix4.h"

namespace three {
namespace math {

Quaternion &Quaternion::set(const Euler &euler, bool emitSignal)
{
  float x = euler.x(), y = euler.y(), z = euler.z();
  Euler::RotationOrder order = euler.order();

  // http://www.mathworks.com/matlabcentral/fileexchange/
  // 	20696-function-to-convert-between-dcm-euler-angles-quaternions-and-euler-vectors/
  //	content/SpinCalc.m

  float c1 = std::cos(x / 2);
  float c2 = std::cos(y / 2);
  float c3 = std::cos(z / 2);

  float s1 = std::sin(x / 2);
  float s2 = std::sin(y / 2);
  float s3 = std::sin(z / 2);

  switch (order) {
    case Euler::RotationOrder::XYZ:
      _x = s1 * c2 * c3 + c1 * s2 * s3;
      _y = c1 * s2 * c3 - s1 * c2 * s3;
      _z = c1 * c2 * s3 + s1 * s2 * c3;
      _w = c1 * c2 * c3 - s1 * s2 * s3;
      break;
    case Euler::RotationOrder::YXZ:
      _x = s1 * c2 * c3 + c1 * s2 * s3;
      _y = c1 * s2 * c3 - s1 * c2 * s3;
      _z = c1 * c2 * s3 - s1 * s2 * c3;
      _w = c1 * c2 * c3 + s1 * s2 * s3;
      break;
    case Euler::RotationOrder::ZXY:
      _x = s1 * c2 * c3 - c1 * s2 * s3;
      _y = c1 * s2 * c3 + s1 * c2 * s3;
      _z = c1 * c2 * s3 + s1 * s2 * c3;
      _w = c1 * c2 * c3 - s1 * s2 * s3;
      break;
    case Euler::RotationOrder::ZYX:
      _x = s1 * c2 * c3 - c1 * s2 * s3;
      _y = c1 * s2 * c3 + s1 * c2 * s3;
      _z = c1 * c2 * s3 - s1 * s2 * c3;
      _w = c1 * c2 * c3 + s1 * s2 * s3;
      break;
    case Euler::RotationOrder::YZX:
      _x = s1 * c2 * c3 + c1 * s2 * s3;
      _y = c1 * s2 * c3 + s1 * c2 * s3;
      _z = c1 * c2 * s3 - s1 * s2 * c3;
      _w = c1 * c2 * c3 - s1 * s2 * s3;
      break;
    case Euler::RotationOrder::XZY:
      _x = s1 * c2 * c3 - c1 * s2 * s3;
      _y = c1 * s2 * c3 - s1 * c2 * s3;
      _z = c1 * c2 * s3 + s1 * s2 * c3;
      _w = c1 * c2 * c3 + s1 * s2 * s3;
  }

  if(emitSignal) onChange.emitSignal(*this);
  return *this;
}

Quaternion::Quaternion(const Vector3 &axis, float angle )
{
  set(axis, angle);
}

Quaternion& Quaternion::set(const Vector3 &axis, float angle, bool emitSignal)
{
  float halfAngle = angle / 2.0f, s = std::sin(halfAngle);

  _x = axis.x() * s;
  _y = axis.y() * s;
  _z = axis.z() * s;
  _w = std::cos( halfAngle );

  if(emitSignal) onChange.emitSignal(*this);
  return *this;
}

Quaternion::Quaternion(const Matrix4 &m) {
  set(m);
}

Quaternion &Quaternion::set(const Matrix4 &m, bool emitSignal)
{
  const float *te = m.elements(),

     m11 = te[0], m12 = te[4], m13 = te[8],
     m21 = te[1], m22 = te[5], m23 = te[9],
     m31 = te[2], m32 = te[6], m33 = te[10],

     trace = m11 + m22 + m33;

  if (trace > 0) {

    float s = 0.5f / std::sqrt(trace + 1.0f);

    _w = 0.25f / s;
    _x = (m32 - m23) * s;
    _y = (m13 - m31) * s;
    _z = (m21 - m12) * s;

  }
  else if (m11 > m22 && m11 > m33) {

    float s = 2.0f * std::sqrt(1.0f + m11 - m22 - m33);

    _w = (m32 - m23) / s;
    _x = 0.25f * s;
    _y = (m12 + m21) / s;
    _z = (m13 + m31) / s;

  }
  else if (m22 > m33) {

    float s = 2.0f * std::sqrt(1.0f + m22 - m11 - m33);

    _w = (m13 - m31) / s;
    _x = (m12 + m21) / s;
    _y = 0.25f * s;
    _z = (m23 + m32) / s;

  }
  else {

    float s = 2.0f * std::sqrt(1.0f + m33 - m11 - m22);

    _w = (m21 - m12) / s;
    _x = (m13 + m31) / s;
    _y = (m23 + m32) / s;
    _z = 0.25f * s;
  }
  if(emitSignal) onChange.emitSignal(*this);
  return *this;
}

Quaternion & Quaternion::setFromUnitVectors(const Vector3 &vFrom, const Vector3 &vTo)
{
  // assumes direction vectors vFrom and vTo are normalized
  Vector3 v1;

  float r = math::dot(vFrom, vTo) + 1;

  if ( r < EPS ) {
    r = 0;

    if (std::abs( vFrom.x() ) > std::abs( vFrom.z() ) ) {
      v1.set( - vFrom.y(), vFrom.x(), 0 );
    }
    else {
      v1.set( 0, - vFrom.z(), vFrom.y() );
    }
  }
  else {
    v1 = cross( vFrom, vTo );
  }

  _x = v1.x();
  _y = v1.y();
  _z = v1.z();
  _w = r;

  return normalize();
}

Quaternion Quaternion::fromUnitVectors(const Vector3 &vFrom, const Vector3 &vTo)
{
  return Quaternion().setFromUnitVectors(vFrom, vTo);
}

}
}