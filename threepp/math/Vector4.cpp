//
// Created by byter on 18.08.17.
//

#include "Vector4.h"

#include "Matrix4.h"
#include "Quaternion.h"

namespace three {
namespace math {

//applyMatrix4
Vector4 &Vector4::operator *=(const Matrix4 &m)
{
  float x = _x, y = _y, z = _z, w = _w;
  const float *e = m.elements();

  _x = e[ 0 ] * x + e[ 4 ] * y + e[ 8 ] * z + e[ 12 ] * w;
  _y = e[ 1 ] * x + e[ 5 ] * y + e[ 9 ] * z + e[ 13 ] * w;
  _z = e[ 2 ] * x + e[ 6 ] * y + e[ 10 ] * z + e[ 14 ] * w;
  _w = e[ 3 ] * x + e[ 7 ] * y + e[ 11 ] * z + e[ 15 ] * w;

  return *this;
}

Vector4 &Vector4::setAxisAngleFromQuaternion(const Quaternion &q)
{
  _w = 2 * std::acos( q.w() );

  float s = std::sqrt( 1 - q.w() * q.w() );

  if ( s < 0.0001 ) {
    _x = 1;
    _y = 0;
    _z = 0;
  }
  else {
    _x = q.x() / s;
    _y = q.y() / s;
    _z = q.z() / s;
  }

  return *this;
}

Vector4 &Vector4::setAxisAngleFromRotationMatrix(const Matrix4 &m)
{
  const float *te = m.elements();

  float angle, x, y, z,		// variables for result
     epsilon = 0.01,		// margin to allow for rounding errors
     epsilon2 = 0.1,		// margin to distinguish between 0 and 180 degrees

     m11 = te[ 0 ], m12 = te[ 4 ], m13 = te[ 8 ],
     m21 = te[ 1 ], m22 = te[ 5 ], m23 = te[ 9 ],
     m31 = te[ 2 ], m32 = te[ 6 ], m33 = te[ 10 ];

  if ( ( std::abs( m12 - m21 ) < epsilon ) &&
       ( std::abs( m13 - m31 ) < epsilon ) &&
       ( std::abs( m23 - m32 ) < epsilon ) ) {

    // singularity found
    // first check for identity matrix which must have +1 for all terms
    // in leading diagonal and zero in other terms

    if ( ( std::abs( m12 + m21 ) < epsilon2 ) &&
         ( std::abs( m13 + m31 ) < epsilon2 ) &&
         ( std::abs( m23 + m32 ) < epsilon2 ) &&
         ( std::abs( m11 + m22 + m33 - 3 ) < epsilon2 ) ) {

      // this singularity is identity matrix so angle = 0

      set( 1, 0, 0, 0 );

      return *this; // zero angle, arbitrary axis
    }

    // otherwise this singularity is angle = 180

    angle = M_PI;

    float xx = ( m11 + 1 ) / 2;
    float yy = ( m22 + 1 ) / 2;
    float zz = ( m33 + 1 ) / 2;
    float xy = ( m12 + m21 ) / 4;
    float xz = ( m13 + m31 ) / 4;
    float yz = ( m23 + m32 ) / 4;

    if ( ( xx > yy ) && ( xx > zz ) ) {

      // m11 is the largest diagonal term

      if ( xx < epsilon ) {

        x = 0;
        y = 0.707106781f;
        z = 0.707106781f;

      } else {

        x = std::sqrt( xx );
        y = xy / x;
        z = xz / x;

      }

    } else if ( yy > zz ) {

      // m22 is the largest diagonal term

      if ( yy < epsilon ) {

        x = 0.707106781f;
        y = 0;
        z = 0.707106781f;

      } else {

        y = std::sqrt( yy );
        x = xy / y;
        z = yz / y;

      }

    } else {

      // m33 is the largest diagonal term so base result on this

      if ( zz < epsilon ) {

        x = 0.707106781f;
        y = 0.707106781f;
        z = 0;

      } else {

        z = std::sqrt( zz );
        x = xz / z;
        y = yz / z;

      }

    }

    set( x, y, z, angle );

    return *this; // return 180 deg rotation
  }

  // as we have reached here there are no singularities so we can handle normally

  float s = std::sqrt( ( m32 - m23 ) * ( m32 - m23 ) +
                     ( m13 - m31 ) * ( m13 - m31 ) +
                     ( m21 - m12 ) * ( m21 - m12 ) ); // used to normalize

  if ( std::abs( s ) < 0.001 ) s = 1;

  // prevent divide by zero, should not happen if matrix is orthogonal and should be
  // caught by singularity test above, but I've left it in just in case

  _x = ( m32 - m23 ) / s;
  _y = ( m13 - m31 ) / s;
  _z = ( m21 - m12 ) / s;
  _w = std::acos( ( m11 + m22 + m33 - 1 ) / 2 );

  return *this;
}

}
}