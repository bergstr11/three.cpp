//
// Created by byter on 18.08.17.
//

#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Spherical.h"
#include "Cylindrical.h"
#include <three/camera/Camera.h>
#include <three/core/BufferAttribute.h>

namespace three {
namespace math {

Vector3 &Vector3::apply(const Matrix4 &m)
{
  float x = _x, y = _y, z = _z;
  const float *e = m.elements();

  float w = 1.0f / ( e[ 3 ] * x + e[ 7 ] * y + e[ 11 ] * z + e[ 15 ] );

  _x = ( e[ 0 ] * x + e[ 4 ] * y + e[ 8 ]  * z + e[ 12 ] ) * w;
  _y = ( e[ 1 ] * x + e[ 5 ] * y + e[ 9 ]  * z + e[ 13 ] ) * w;
  _z = ( e[ 2 ] * x + e[ 6 ] * y + e[ 10 ] * z + e[ 14 ] ) * w;
}

//apply quaternion
Vector3 &Vector3::apply(const Quaternion &q)
{
  float x = _x, y = _y, z = _z;
  float qx = q.x(), qy = q.y(), qz = q.z(), qw = q.w();

  // calculate quat * vector

  float ix =  qw * x + qy * z - qz * y;
  float iy =  qw * y + qz * x - qx * z;
  float iz =  qw * z + qx * y - qy * x;
  float iw = - qx * x - qy * y - qz * z;

  // calculate result * inverse quat

  _x = ix * qw + iw * - qx + iy * - qz - iz * - qy;
  _y = iy * qw + iw * - qy + iz * - qx - ix * - qz;
  _z = iz * qw + iw * - qz + ix * - qy - iy * - qx;
}

Vector3 &Vector3::project(const Camera &camera)
{
  apply(camera.projectionMatrix() * camera.matrixWorld().inverse());
  return *this;
}

Vector3 &Vector3::unproject(const Camera &camera)
{
  apply(camera.matrixWorld() * camera.projectionMatrix().inverse());
  return *this;
}

// input: THREE.Matrix4 affine matrix
// vector interpreted as a direction
Vector3 &Vector3::transformDirection(const Matrix4 & m)
{
  float x = _x, y = _y, z = _z;
  const float *e = m.elements();

  _x = e[ 0 ] * x + e[ 4 ] * y + e[ 8 ]  * z;
  _y = e[ 1 ] * x + e[ 5 ] * y + e[ 9 ]  * z;
  _z = e[ 2 ] * x + e[ 6 ] * y + e[ 10 ] * z;

  normalize();
  return *this;
}

Vector3 Vector3::fromSpherical(const Spherical &s)
{
  float sinPhiRadius = std::sin( s.phi() ) * s.radius();

  float x = sinPhiRadius * std::sin( s.theta() );
  float y = std::cos( s.phi() ) * s.radius();
  float z = sinPhiRadius * std::cos( s.theta() );

  return Vector3(x, y, z);
}

Vector3 Vector3::fromCylindrical(const Cylindrical &c)
{
  float x = c.radius() * std::sin( c.theta() );
  float y = c.y();
  float z = c.radius() * std::cos( c.theta() );

  return Vector3(x, y, z);
}

Vector3 Vector3::fromMatrixPosition(const Matrix4 &m)
{
  const float *e = m.elements();

  float x = e[ 12 ];
  float y = e[ 13 ];
  float z = e[ 14 ];

  return Vector3(x, y, z);
}

Vector3 Vector3::fromMatrixColumn(const Matrix4 &m, unsigned index)
{
  return fromArray( m.elements(), index * 4 );
}

Vector3 Vector3::fromMatrixScale(const Matrix4 &m)
{
  float sx = fromMatrixColumn( m, 0 ).length();
  float sy = fromMatrixColumn( m, 1 ).length();
  float sz = fromMatrixColumn( m, 2 ).length();

  return Vector3(sx, sy, sz);
}

Vector3 &Vector3::apply(const Matrix3 &m)
{
  const float *e = m.elements();

  _x = e[ 0 ] * _x + e[ 3 ] * _y + e[ 6 ] * _z;
  _y = e[ 1 ] * _x + e[ 4 ] * _y + e[ 7 ] * _z;
  _z = e[ 2 ] * _x + e[ 5 ] * _y + e[ 8 ] * _z;
}

Vector3 Vector3::fromBufferAttribute(const BufferAttributeT<float> &attribute, unsigned index)
{
  return Vector3(attribute.get_x( index ), attribute.get_y( index ), attribute.get_z( index ));
}

}
}