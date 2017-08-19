//
// Created by byter on 18.08.17.
//

#include "Box3.h"
#include "Sphere.h"
#include "Plane.h"

namespace three {
namespace math {

Sphere Box3::getBoundingSphere() const
{
  return Sphere {getCenter(), getSize().length() * 0.5f};
}

bool Box3::intersectsSphere(const Sphere &sphere) const
{
  // Find the point on the AABB closest to the sphere center.
  Vector3 closestPoint = clampPoint(sphere.center());

  // If that point is inside the sphere, the AABB and sphere intersect.
  return closestPoint.distanceToSquared( sphere.center() ) <= ( sphere.radius() * sphere.radius() );
}

bool Box3::intersectsPlane(const Plane &plane ) const
{
  // We compute the minimum and maximum dot product values. If those values
  // are on the same side (back or front) of the plane, then there is no intersection.
  float min, max;

  if (plane.normal().x() > 0 ) {

    min = plane.normal().x() * _min.x();
    max = plane.normal().x() * _max.x();

  } else {

    min = plane.normal().x() * _max.x();
    max = plane.normal().x() * _min.x();

  }

  if ( plane.normal().y() > 0 ) {

    min += plane.normal().y() * _min.y();
    max += plane.normal().y() * _max.y();

  } else {

    min += plane.normal().y() * _max.y();
    max += plane.normal().y() * _min.y();

  }

  if ( plane.normal().z() > 0 ) {

    min += plane.normal().z() * _min.z();
    max += plane.normal().z() * _max.z();

  } else {

    min += plane.normal().z() * _max.z();
    max += plane.normal().z() * _min.z();

  }

  return ( min <= plane.constant() && max >= plane.constant() );
}

}
}