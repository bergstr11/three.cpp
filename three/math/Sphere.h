//
// Created by byter on 13.08.17.
//

#ifndef THREE_QT_SPHERE_H
#define THREE_QT_SPHERE_H

#include "Vector3.h"
#include <vector>

namespace three {
namespace math {

class Box3;
class Plane;

static float computeRadius(const std::vector<Vector3> &points, const Vector3 &center)
{
  float maxRadiusSq = 0;

  for (const Vector3 &point : points) {
    maxRadiusSq = std::max(maxRadiusSq, center.distanceToSquared(point));
  }

  return std::sqrt(maxRadiusSq);
}

class Sphere
{
  Vector3 _center;
  float _radius;

public:
  Sphere(const Vector3 &center, const float radius) : _center(center), _radius(radius) {}

  Sphere(const Sphere &sphere) : _center(sphere._center), _radius(sphere._radius) {}

  Sphere(const std::vector<Vector3> &points, const Vector3 &center)
     : _center(center), _radius(computeRadius(points, center))
  {}

  Sphere() : _radius(0.0f), _center({0.0f, 0.0f, 0.0f}){}

  Sphere &set(const std::vector<Vector3> &points)
  {
    _radius = computeRadius(points, _center);
  }

  const Vector3 &center() const {return _center;}

  float radius() const {return _radius;}

  bool isEmpty() const
  {
    return _radius <= 0;
  }

  bool containsPoint(const Vector3 &point)
  {
    return (point.distanceToSquared(_center) <= (_radius * _radius));
  }

  float distanceToPoint(const Vector3 &point)
  {
    return point.distanceTo(_center) - _radius;
  }

  bool intersectsSphere(const Sphere &sphere)
  {
    float radiusSum = _radius + sphere._radius;
    return sphere._center.distanceToSquared(_center) <= (radiusSum * radiusSum);
  }

  bool intersectsBox(const Box3 &box) const;

  bool intersectsPlane(const Plane &plane) const;

  Vector3 clampPoint(const Vector3 &point)
  {

    float deltaLengthSq = _center.distanceToSquared(point);

    Vector3 result(point);

    if (deltaLengthSq > (_radius * _radius)) {

      result -= _center;
      result.normalize();
      result *= _radius;
      result += _center;

    }
    return result;
  }

  Box3 getBoundingBox();

  Sphere &apply(const Matrix4 &matrix)
  {
    _center.apply(matrix);
    _radius = _radius * matrix.getMaxScaleOnAxis();

    return *this;
  }

  Sphere &translate(const Vector3 &offset)
  {
    _center += offset;
    return *this;
  }

  bool operator ==(const Sphere &sphere)
  {
    return sphere._center == _center && sphere._radius == _radius;
  }
};

}
}
#endif //THREE_QT_SPHERE_H
