//
// Created by byter on 6/10/18.
//

#include "Circle3.h"
#include <algorithm>
#include <stdexcept>

namespace three {
namespace math {

using namespace std;

Circle3::Circle3(const Vector3 &center, const Vector3 &normal, float radius)
   : _center(center), _normal(normal), _radius(radius)
{
  Vector3 vp(0.0f, 0.0f, 0.0f);

  if (_normal.x() != 0 || _normal.y() != 0) {
    vp.z() = 1.0f;
  }
  else if (_normal.x() != 0 || _normal.z() != 0) {
    vp.y() = 1.0f;
  }
  else if (_normal.y() != 0 || _normal.z() != 0) {
    vp.x() = 1.0f;
  }
  else {
    throw invalid_argument("empty normal");
  }

  v = cross(_normal, vp).normalized();
  w = cross(v, _normal);
}

Vector3 Circle3::pointAt(float angle)
{
  float xv = std::cos(angle);
  float yv = std::sin(angle);

  // Return center + r * (V * cos(a) + W * sin(a))
  Vector3 r1 = v * _radius * xv;
  Vector3 r2 = w * _radius * yv;

  return Vector3(_center.x() + r1.x() + r2.x(),
                 _center.y() + r1.y() + r2.y(),
                 _center.z() + r1.z() + r2.z());
}

}
}