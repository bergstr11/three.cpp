//
// Created by byter on 11.09.17.
//

#include "Frustum.h"

#include <threepp/objects/Sprite.h>
#include <iostream>

namespace three {
namespace math {

using namespace std;

bool Frustum::intersectsObject(const Object3D &object) const
{
  if (object.geometry()->boundingSphere().isEmpty())
    object.geometry()->computeBoundingSphere();

  Sphere sphere = object.geometry()->boundingSphere();
  sphere.apply(object.matrixWorld());

  return intersectsSphere( sphere );
}

bool Frustum::intersectsSprite(const Sprite &sprite) const
{
  Sphere sphere(Vector3(0, 0, 0), 0.7071067811865476f);
  sphere.apply(sprite.matrixWorld());

  return intersectsSphere( sphere );
}

bool Frustum::intersectsSphere(const Sphere &sphere) const
{
  float negRadius = -sphere.radius();

  for (const Plane &plane : _planes) {

    float distance = plane.distanceToPoint(sphere.center());

    if (distance < negRadius) {
      return false;
    }
  }

  return true;
}

bool Frustum::intersectsBox(const Box3 &box) const
{
  for (const Plane &plane : _planes) {

    Vector3 p1(plane.normal().x() > 0 ? box.min().x() : box.max().x(),
               plane.normal().y() > 0 ? box.min().y() : box.max().y(),
               plane.normal().z() > 0 ? box.min().z() : box.max().z());

    Vector3 p2(plane.normal().x() > 0 ? box.max().x() : box.min().x(),
               plane.normal().y() > 0 ? box.max().y() : box.min().y(),
               plane.normal().z() > 0 ? box.max().z() : box.min().z());

    float d1 = plane.distanceToPoint(p1);
    float d2 = plane.distanceToPoint(p2);

    // if both outside plane, no intersection
    if (d1 < 0 && d2 < 0) {
      return false;
    }
  }

  return true;
}


}
}