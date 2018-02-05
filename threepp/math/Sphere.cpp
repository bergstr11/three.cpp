//
// Created by byter on 19.08.17.
//
#include "Sphere.h"
#include "Box3.h"
#include "Plane.h"

namespace three {
namespace math {

bool Sphere::intersectsBox(const Box3 &box) const
{
  return box.intersectsSphere(*this);
}

bool Sphere::intersectsPlane(const Plane &plane) const
{
  return std::abs(plane.distanceToPoint(_center)) <= _radius;
}

Box3 Sphere::getBoundingBox()
{
  Box3 box(_center, _center);
  box.expandByScalar(_radius);

  return box;
}

}
}