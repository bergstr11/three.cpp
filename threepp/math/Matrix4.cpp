//
// Created by byter on 18.08.17.
//

#include "Matrix4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace three {
namespace math {

const float Matrix4::IDENTITY[] = {
   1, 0, 0, 0,
   0, 1, 0, 0,
   0, 0, 1, 0,
   0, 0, 0, 1
};

Matrix3 Matrix4::normalMatrix() const
{
  const float *me = _elements;

  return Matrix3(
     me[ 0 ], me[ 4 ], me[  8 ],
     me[ 1 ], me[ 5 ], me[  9 ],
     me[ 2 ], me[ 6 ], me[ 10 ]

  ).inverted().transpose();
}

Matrix4 &Matrix4::scale(const Vector3 &v)
{
  float *te = _elements;
  float x = v.x(), y = v.y(), z = v.z();

  te[ 0 ] *= x; te[ 4 ] *= y; te[ 8 ] *= z;
  te[ 1 ] *= x; te[ 5 ] *= y; te[ 9 ] *= z;
  te[ 2 ] *= x; te[ 6 ] *= y; te[ 10 ] *= z;
  te[ 3 ] *= x; te[ 7 ] *= y; te[ 11 ] *= z;

  return *this;
}

Matrix4 &Matrix4::setPosition(const Vector3 &v)
{
  float *te = _elements;

  te[12] = v.x();
  te[13] = v.y();
  te[14] = v.z();

  return *this;
}

Vector3 Matrix4::getPosition()
{
  return Vector3(_elements[12], _elements[13], _elements[14]);
}

Matrix4 &Matrix4::setBasis(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis)
{
  set(
     xAxis.x(), yAxis.x(), zAxis.x(), 0,
     xAxis.y(), yAxis.y(), zAxis.y(), 0,
     xAxis.z(), yAxis.z(), zAxis.z(), 0,
     0, 0, 0, 1
  );

  return *this;
}

Matrix4 Matrix4::rotation(const Matrix4 &m)
{
  Matrix4 rotation;

  float *te = rotation._elements;
  const float *me = m._elements;

  float scaleX = 1 / Vector3::fromMatrixColumn(m, 0).length();
  float scaleY = 1 / Vector3::fromMatrixColumn(m, 1).length();
  float scaleZ = 1 / Vector3::fromMatrixColumn(m, 2).length();

  te[0] = me[0] * scaleX;
  te[1] = me[1] * scaleX;
  te[2] = me[2] * scaleX;

  te[4] = me[4] * scaleY;
  te[5] = me[5] * scaleY;
  te[6] = me[6] * scaleY;

  te[8] = me[8] * scaleZ;
  te[9] = me[9] * scaleZ;
  te[10] = me[10] * scaleZ;

  return rotation;
}

Matrix4 Matrix4::rotation(const Euler &euler)
{
  Matrix4 rotation;
  float *te = rotation._elements;

  float x = euler.x(), y = euler.y(), z = euler.z();
  float a = std::cos(x), b = std::sin(x);
  float c = std::cos(y), d = std::sin(y);
  float e = std::cos(z), f = std::sin(z);

  switch(euler.order()) {
    case Euler::RotationOrder::XYZ: {
      float ae = a * e, af = a * f, be = b * e, bf = b * f;

      te[0] = c * e;
      te[4] = -c * f;
      te[8] = d;

      te[1] = af + be * d;
      te[5] = ae - bf * d;
      te[9] = -b * c;

      te[2] = bf - ae * d;
      te[6] = be + af * d;
      te[10] = a * c;
      break;
    }
    case Euler::RotationOrder::YXZ: {
      float ce = c * e, cf = c * f, de = d * e, df = d * f;

      te[0] = ce + df * b;
      te[4] = de * b - cf;
      te[8] = a * d;

      te[1] = a * f;
      te[5] = a * e;
      te[9] = -b;

      te[2] = cf * b - de;
      te[6] = df + ce * b;
      te[10] = a * c;
      break;
    }
    case Euler::RotationOrder::ZXY: {
      float ce = c * e, cf = c * f, de = d * e, df = d * f;

      te[0] = ce - df * b;
      te[4] = -a * f;
      te[8] = de + cf * b;

      te[1] = cf + de * b;
      te[5] = a * e;
      te[9] = df - ce * b;

      te[2] = -a * d;
      te[6] = b;
      te[10] = a * c;
      break;
    }
    case Euler::RotationOrder::ZYX: {
      float ae = a * e, af = a * f, be = b * e, bf = b * f;

      te[0] = c * e;
      te[4] = be * d - af;
      te[8] = ae * d + bf;

      te[1] = c * f;
      te[5] = bf * d + ae;
      te[9] = af * d - be;

      te[2] = -d;
      te[6] = b * c;
      te[10] = a * c;
      break;
    }
    case Euler::RotationOrder::YZX: {
      float ac = a * c, ad = a * d, bc = b * c, bd = b * d;

      te[0] = c * e;
      te[4] = bd - ac * f;
      te[8] = bc * f + ad;

      te[1] = f;
      te[5] = a * e;
      te[9] = -b * e;

      te[2] = -d * e;
      te[6] = ad * f + bc;
      te[10] = ac - bd * f;
      break;
    }
    case Euler::RotationOrder::XZY: {
      float ac = a * c, ad = a * d, bc = b * c, bd = b * d;

      te[0] = c * e;
      te[4] = -f;
      te[8] = d * e;

      te[1] = ac * f + bd;
      te[5] = a * e;
      te[9] = ad * f - bc;

      te[2] = bc * f - ad;
      te[6] = b * e;
      te[10] = bd * f + ac;
      break;
    }
  }

  // last column
  te[3] = 0;
  te[7] = 0;
  te[11] = 0;

  // bottom row
  te[12] = 0;
  te[13] = 0;
  te[14] = 0;
  te[15] = 1;

  return rotation;
}

Matrix4 Matrix4::rotation(const Quaternion &q)
{
  Matrix4 rotation;
  float *te = rotation._elements;

  float x = q.x(), y = q.y(), z = q.z(), w = q.w();
  float x2 = x + x, y2 = y + y, z2 = z + z;
  float xx = x * x2, xy = x * y2, xz = x * z2;
  float yy = y * y2, yz = y * z2, zz = z * z2;
  float wx = w * x2, wy = w * y2, wz = w * z2;

  te[0] = 1.0f - (yy + zz);
  te[4] = xy - wz;
  te[8] = xz + wy;

  te[1] = xy + wz;
  te[5] = 1.0f - (xx + zz);
  te[9] = yz - wx;

  te[2] = xz - wy;
  te[6] = yz + wx;
  te[10] = 1.0f - (xx + yy);

  // last column
  te[3] = 0;
  te[7] = 0;
  te[11] = 0;

  // bottom row
  te[12] = 0;
  te[13] = 0;
  te[14] = 0;
  te[15] = 1;

  return rotation;
}

Matrix4 &Matrix4::lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
{
  float *te = _elements;

  Vector3 z = eye - target;

  if (z.lengthSq() == 0) {
    // eye and target are in the same position
    z.z() = 1;
  }

  z.normalize();
  Vector3 x = cross(up, z);

  if (x.lengthSq() == 0) {

    // up and z are parallel
    if (std::abs(up.z()) == 1) {
      z.x() += 0.0001f;
    }
    else {
      z.z() += 0.0001f;
    }

    z.normalize();
    x = cross(up, z);
  }

  x.normalize();
  Vector3 y = cross(z, x);

  te[0] = x.x();
  te[4] = y.x();
  te[8] = z.x();
  te[1] = x.y();
  te[5] = y.y();
  te[9] = z.y();
  te[2] = x.z();
  te[6] = y.z();
  te[10] = z.z();

  return *this;
}

Matrix4 Matrix4::axisRotation(const Vector3 &axis, float angle)
{
  // Based on http://www.gamedev.net/reference/articles/article1199.asp
  float c = std::cos(angle);
  float s = std::sin(angle);
  float t = 1 - c;
  float x = axis.x(), y = axis.y(), z = axis.z();
  float tx = t * x, ty = t * y;

  return Matrix4(
     tx * x + c, tx * y - s * z, tx * z + s * y, 0,
     tx * y + s * z, ty * y + c, ty * z - s * x, 0,
     tx * z - s * y, ty * z + s * x, t * z * z + c, 0,
     0, 0, 0, 1
  );
}

void Matrix4::decompose(Vector3 &position, Quaternion &rotation, Vector3&scale)
{
  float *te = _elements;

  float sx = Vector3(te[0], te[1], te[2]).length();
  float sy = Vector3(te[4], te[5], te[6]).length();
  float sz = Vector3(te[8], te[9], te[10]).length();

  // if determine is negative, we need to invert one scale
  float det = determinant();
  if (det < 0) sx = -sx;

  position.x() = te[12];
  position.y() = te[13];
  position.z() = te[14];

  // scale the rotation part
  Matrix4 matrix(*this);

  float invSX = 1.0f / sx;
  float invSY = 1.0f / sy;
  float invSZ = 1.0f / sz;

  matrix._elements[0] *= invSX;
  matrix._elements[1] *= invSX;
  matrix._elements[2] *= invSX;

  matrix._elements[4] *= invSY;
  matrix._elements[5] *= invSY;
  matrix._elements[6] *= invSY;

  matrix._elements[8] *= invSZ;
  matrix._elements[9] *= invSZ;
  matrix._elements[10] *= invSZ;

  rotation = Quaternion(matrix);

  scale.x() = sx;
  scale.y() = sy;
  scale.z() = sz;
}

}
}
