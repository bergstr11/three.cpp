//
// Created by byter on 18.08.17.
//
#include "Math.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace three {
namespace math {

void decompose(const Matrix4 &matrix, Vector3 &position, Quaternion &quaternion, Vector3 &scale )
{
  const float *te = matrix.elements();

  float sx = Vector3( te[ 0 ], te[ 1 ], te[ 2 ] ).length();
  float sy = Vector3( te[ 4 ], te[ 5 ], te[ 6 ] ).length();
  float sz = Vector3( te[ 8 ], te[ 9 ], te[ 10 ] ).length();

  // if determine is negative, we need to invert one scale
  float det = matrix.determinant();
  if ( det < 0 ) sx = - sx;

  position.set(te[ 12 ], te[ 13 ], te[ 14 ]);

  // scale the rotation part
  Matrix4 m(matrix);

  float invSX = 1 / sx;
  float invSY = 1 / sy;
  float invSZ = 1 / sz;

  float *me = m.elements();
  me[ 0 ] *= invSX;
  me[ 1 ] *= invSX;
  me[ 2 ] *= invSX;

  me[ 4 ] *= invSY;
  me[ 5 ] *= invSY;
  me[ 6 ] *= invSY;

  me[ 8 ] *= invSZ;
  me[ 9 ] *= invSZ;
  me[ 10 ] *= invSZ;

  quaternion.set(m);

  scale.set(sx, sy, sz);
}

}
}