//
// Created by byter on 10.09.17.
//
#include "impl/raycast.h"

namespace three {

void intersectObject(const Object3D &object, const Raycaster &raycaster, std::vector<Intersection> &intersects, bool recursive )
{
  if (!object.visible()) return;

  object.raycast( raycaster, intersects );

  if (recursive ) {
    const std::vector<Object3D::Ptr> &children = object.children();

    for (auto child : children) {
      intersectObject(*child, raycaster, intersects, true);
    }
  }
}

std::vector<Intersection> Raycaster::intersectObject(const Object3D &object, bool recursive )
{
  std::vector<Intersection> intersects;

  three::intersectObject( object, *this, intersects, recursive );

  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance > b.distance;});

  return intersects;
}

std::vector<Intersection> Raycaster::intersectObjects(const std::vector<Object3D::Ptr> objects,
                                           bool recursive )
{
  std::vector<Intersection> intersects;

  for (auto obj : objects) {
    three::intersectObject( *obj, *this, intersects, recursive );
  }
  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});

  return intersects;
}

}
