//
// Created by byter on 16.08.17.
//

#ifndef THREE_QT_MATRIX4_H
#define THREE_QT_MATRIX4_H

#include <stdexcept>
#include <cstring>
#include <cmath>
#include "Matrix3.h"

namespace three {
namespace math {

class Vector3;
class Quaternion;
class Euler;

class Matrix4
{
  static const float IDENTITY[];

  float _elements[16];

public:
  Matrix3 normalMatrix() const;

  Matrix4()
  {
    std::memcpy(_elements, IDENTITY, sizeof(_elements));
  }

  Matrix4(float n11, float n12, float n13, float n14, float n21, float n22, float n23, float n24, float n31, float n32, float n33, float n34, float n41, float n42, float n43, float n44)
  {
    set(n11, n12, n13, n14, n21, n22, n23, n24, n31, n32, n33, n34, n41, n42, n43, n44);
  }

  Matrix4(const Matrix4 &matrix)
  {
    std::memcpy(_elements, matrix._elements, sizeof(_elements));
  }

  Matrix4(const Vector3 &eye, const Vector3 &target, const Vector3 &up) {
    lookAt(eye, target, up);
  }

  const float * elements() const {return _elements;}

  float * elements() {return _elements;}

  Matrix4 &set(float n11, float n12, float n13, float n14, float n21, float n22, float n23, float n24, float n31, float n32, float n33, float n34, float n41, float n42, float n43, float n44)
  {
    float *te = _elements;

    te[0] = n11;
    te[4] = n12;
    te[8] = n13;
    te[12] = n14;
    te[1] = n21;
    te[5] = n22;
    te[9] = n23;
    te[13] = n24;
    te[2] = n31;
    te[6] = n32;
    te[10] = n33;
    te[14] = n34;
    te[3] = n41;
    te[7] = n42;
    te[11] = n43;
    te[15] = n44;

    return *this;
  }

  static Matrix4 identity()
  {
    Matrix4 matrix4;
    std::memcpy(matrix4._elements, IDENTITY, sizeof(_elements));
    return matrix4;
  }

  Matrix4 &setPosition(const Matrix4 &m)
  {
    float *te = _elements;
    const float *me = m._elements;

    te[12] = me[12];
    te[13] = me[13];
    te[14] = me[14];

    return *this;
  }

  Matrix4 &setBasis(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis);

  static Matrix4 rotation(const Matrix4 &m);

  static Matrix4 rotation(const Euler &euler);

  static Matrix4 rotation(const Quaternion &q);

  Matrix4 &lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up);

  Matrix4 &multiply(const Matrix4 &m1, const Matrix4 &m2)
  {
    const float *ae = m1._elements;
    const float *be = m2._elements;
    float *te = _elements;

    float a11 = ae[0], a12 = ae[4], a13 = ae[8], a14 = ae[12];
    float a21 = ae[1], a22 = ae[5], a23 = ae[9], a24 = ae[13];
    float a31 = ae[2], a32 = ae[6], a33 = ae[10], a34 = ae[14];
    float a41 = ae[3], a42 = ae[7], a43 = ae[11], a44 = ae[15];

    float b11 = be[0], b12 = be[4], b13 = be[8], b14 = be[12];
    float b21 = be[1], b22 = be[5], b23 = be[9], b24 = be[13];
    float b31 = be[2], b32 = be[6], b33 = be[10], b34 = be[14];
    float b41 = be[3], b42 = be[7], b43 = be[11], b44 = be[15];

    te[0] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
    te[4] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
    te[8] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
    te[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

    te[1] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
    te[5] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
    te[9] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
    te[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

    te[2] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
    te[6] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
    te[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
    te[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

    te[3] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
    te[7] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
    te[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
    te[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

    return *this;
  }

  Matrix4 &operator *=(const Matrix4 &m)
  {
    multiply(*this, m);
  }

  Matrix4 &premultiply(const Matrix4 &m)
  {
    return multiply(m, *this);
  }

  Matrix4 &operator *=(float s)
  {
    float *te = _elements;

    te[0] *= s;
    te[4] *= s;
    te[8] *= s;
    te[12] *= s;
    te[1] *= s;
    te[5] *= s;
    te[9] *= s;
    te[13] *= s;
    te[2] *= s;
    te[6] *= s;
    te[10] *= s;
    te[14] *= s;
    te[3] *= s;
    te[7] *= s;
    te[11] *= s;
    te[15] *= s;

    return *this;
  }

  float determinant() const
  {
    const float *te = _elements;

    float n11 = te[0], n12 = te[4], n13 = te[8], n14 = te[12];
    float n21 = te[1], n22 = te[5], n23 = te[9], n24 = te[13];
    float n31 = te[2], n32 = te[6], n33 = te[10], n34 = te[14];
    float n41 = te[3], n42 = te[7], n43 = te[11], n44 = te[15];

    //TODO: make this more efficient
    //( based on http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm )

    return (
       n41 * (
          +n14 * n23 * n32
          - n13 * n24 * n32
          - n14 * n22 * n33
          + n12 * n24 * n33
          + n13 * n22 * n34
          - n12 * n23 * n34
       ) +
       n42 * (
          +n11 * n23 * n34
          - n11 * n24 * n33
          + n14 * n21 * n33
          - n13 * n21 * n34
          + n13 * n24 * n31
          - n14 * n23 * n31
       ) +
       n43 * (
          +n11 * n24 * n32
          - n11 * n22 * n34
          - n14 * n21 * n32
          + n12 * n21 * n34
          + n14 * n22 * n31
          - n12 * n24 * n31
       ) +
       n44 * (
          -n13 * n22 * n31
          - n11 * n23 * n32
          + n11 * n22 * n33
          + n13 * n21 * n32
          - n12 * n21 * n33
          + n12 * n23 * n31
       )
    );
  }

  Matrix4 &transpose()
  {
    float *te = _elements;
    float tmp;

    tmp = te[1];
    te[1] = te[4];
    te[4] = tmp;
    tmp = te[2];
    te[2] = te[8];
    te[8] = tmp;
    tmp = te[6];
    te[6] = te[9];
    te[9] = tmp;

    tmp = te[3];
    te[3] = te[12];
    te[12] = tmp;
    tmp = te[7];
    te[7] = te[13];
    te[13] = tmp;
    tmp = te[11];
    te[11] = te[14];
    te[14] = tmp;

    return *this;
  }

  Matrix4 &setPosition(const Vector3 &v);

  Matrix4 inverse() const
  {
    // based on http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
    Matrix4 inv;

    float *te = inv._elements;
    const float *me = _elements,

       n11 = me[0], n21 = me[1], n31 = me[2], n41 = me[3],
       n12 = me[4], n22 = me[5], n32 = me[6], n42 = me[7],
       n13 = me[8], n23 = me[9], n33 = me[10], n43 = me[11],
       n14 = me[12], n24 = me[13], n34 = me[14], n44 = me[15],

       t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44,
       t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44,
       t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44,
       t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;

    if (det == 0) {
      throw new std::invalid_argument("THREE.Matrix4: inverse() can't invert matrix, determinant is 0");
    }

    float detInv = 1 / det;

    te[0] = t11 * detInv;
    te[1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * detInv;
    te[2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * detInv;
    te[3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * detInv;

    te[4] = t12 * detInv;
    te[5] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * detInv;
    te[6] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * detInv;
    te[7] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * detInv;

    te[8] = t13 * detInv;
    te[9] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * detInv;
    te[10] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * detInv;
    te[11] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * detInv;

    te[12] = t14 * detInv;
    te[13] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * detInv;
    te[14] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * detInv;
    te[15] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * detInv;

    return inv;
  }

  Matrix4 &scale(const Vector3 &v);

  float getMaxScaleOnAxis() const
  {
    const float *te = _elements;

    float scaleXSq = te[0] * te[0] + te[1] * te[1] + te[2] * te[2];
    float scaleYSq = te[4] * te[4] + te[5] * te[5] + te[6] * te[6];
    float scaleZSq = te[8] * te[8] + te[9] * te[9] + te[10] * te[10];

    return std::sqrt(std::max(std::max(scaleXSq, scaleYSq), scaleZSq));
  }

  static Matrix4 translation(float x, float y, float z)
  {
    return Matrix4(
       1, 0, 0, x,
       0, 1, 0, y,
       0, 0, 1, z,
       0, 0, 0, 1
    );
  }

  static Matrix4 rotationX(float theta)
  {
    float c = std::cos(theta), s = std::sin(theta);

    return Matrix4(
       1, 0, 0, 0,
       0, c, -s, 0,
       0, s, c, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4 rotationY(float theta)
  {
    float c = std::cos(theta), s = std::sin(theta);

    return Matrix4(
       c, 0, s, 0,
       0, 1, 0, 0,
       -s, 0, c, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4 rotationZ(float theta)
  {
    float c = std::cos(theta), s = std::sin(theta);

    return Matrix4(
       c, -s, 0, 0,
       s, c, 0, 0,
       0, 0, 1, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4 axisRotation(const Vector3 &axis, float angle);

  static Matrix4 scaling(float x, float y, float z)
  {
    return Matrix4(
       x, 0, 0, 0,
       0, y, 0, 0,
       0, 0, z, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4 shear(float x, float y, float z)
  {
    return Matrix4(
       1, y, z, 0,
       x, 1, z, 0,
       x, y, 1, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4 compose(const Vector3 &position, const Quaternion &quaternion, const Vector3 &scale)
  {
    return Matrix4::rotation(quaternion).scale(scale).setPosition(position);
  }

  void decompose(Vector3 &position, Quaternion &rotation, Vector3&scale);

  static Matrix4 perspective(float left, float right, float top, float bottom, float near, float far)
  {
    Matrix4 m4;
    float *te = m4._elements;
    float x = 2 * near / (right - left);
    float y = 2 * near / (top - bottom);

    float a = (right + left) / (right - left);
    float b = (top + bottom) / (top - bottom);
    float c = -(far + near) / (far - near);
    float d = -2 * far * near / (far - near);

    te[0] = x;
    te[4] = 0;
    te[8] = a;
    te[12] = 0;
    te[1] = 0;
    te[5] = y;
    te[9] = b;
    te[13] = 0;
    te[2] = 0;
    te[6] = 0;
    te[10] = c;
    te[14] = d;
    te[3] = 0;
    te[7] = 0;
    te[11] = -1;
    te[15] = 0;

    return m4;
  }

  static Matrix4 orthographic(float left, float right, float top, float bottom, float near, float far)
  {
    Matrix4 m4;
    float *te = m4._elements;
    float w = 1.0f / (right - left);
    float h = 1.0f / (top - bottom);
    float p = 1.0f / (far - near);

    float x = (right + left) * w;
    float y = (top + bottom) * h;
    float z = (far + near) * p;

    te[0] = 2 * w;
    te[4] = 0;
    te[8] = 0;
    te[12] = -x;
    te[1] = 0;
    te[5] = 2 * h;
    te[9] = 0;
    te[13] = -y;
    te[2] = 0;
    te[6] = 0;
    te[10] = -2 * p;
    te[14] = -z;
    te[3] = 0;
    te[7] = 0;
    te[11] = 0;
    te[15] = 1;

    return m4;
  }

  bool operator ==(const Matrix4 &matrix) const
  {
    const float *te = _elements;
    const float *me = matrix._elements;

    return std::memcmp(_elements, matrix._elements, sizeof(_elements)) == 0;
  }

  Matrix4(float *array, unsigned offset)
  {
    std::memcpy(_elements, array+offset, sizeof(_elements));
  }

  void writeTo(float *array, unsigned offset)
  {
    std::memcpy(array+offset, _elements, sizeof(_elements));
  }
};

inline Matrix4 operator *(const Matrix4 &m1, const Matrix4 &m2)
{
  Matrix4 result(m1);
  result *= m2;
  return result;
}

}
}
#endif //THREE_QT_MATRIX4_H
