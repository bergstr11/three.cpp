//
// Created by byter on 08.09.17.
//

#include "Vector2.h"
#include "Matrix3.h"
#include <threepp/core/BufferAttribute.h>

namespace three {
namespace math {

Vector2 Vector2::fromBufferAttribute(const BufferAttributeT<float> &attribute, unsigned index)
{
  return Vector2(attribute.get_x( index ), attribute.get_y( index ));
}

Vector2 &Vector2::apply(const Matrix3 &m)
{
  const float *e = m.elements();

  _x = e[ 0 ] * _x + e[ 3 ] * _y + e[ 6 ];
  _y = e[ 1 ] * _x + e[ 4 ] * _y + e[ 7 ];

  return *this;
}

}
}