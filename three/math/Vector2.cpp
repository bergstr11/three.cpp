//
// Created by byter on 08.09.17.
//

#include "Vector2.h"
#include <core/BufferAttribute.h>

namespace three {
namespace math {

Vector2 Vector2::fromBufferAttribute(const BufferAttribute<float> &attribute, unsigned index)
{
  return Vector2(attribute.get_x( index ), attribute.get_y( index ));
}

}
}