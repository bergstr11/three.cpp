//
// Created by byter on 6/10/18.
//

#ifndef THREE_PP_CIRCLE3_H
#define THREE_PP_CIRCLE3_H

#include "Vector3.h"

namespace three {
namespace math {

class Circle3
{
  const Vector3 _center, _normal;
  const float _radius;

  Vector3 v, w;

public:
  Circle3(const Vector3 &center, const Vector3 &normal, float radius);

  Vector3 pointAt(float angle);
};

}
}
#endif //THREE_PP_CIRCLE3_H
