//
// Created by byter on 10.09.17.
//
#include "impl/raycast.h"

namespace three {

void intersectObject(Object3D &object,
                     const Raycaster &raycaster,
                     std::vector<Intersection> &intersects,
                     bool recursive )
{
  if (!object.visible()) return;

  object.raycast( raycaster, intersects );

  if (recursive ) {
    const std::vector<Object3D::Ptr> &children = object.children();

    for (auto child : children) {
      intersectObject(*child, raycaster, intersects, recursive);
    }
  }
}

std::vector<Intersection> Raycaster::intersectObject(Object3D &object, bool recursive )
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

std::vector<math::Ray> Raycaster::createCircularBundle(
   const math::Ray &ray, float radius, unsigned radialSegments)
{
  std::vector<math::Ray> rays;

  //center ray first
  rays.push_back(ray);

  const auto &center = ray.origin();
  const auto &normal = ray.direction();

  //then the circle rays
  float segment = 2.0f * (float)M_PI / radialSegments;
  for (unsigned s = 0; s < radialSegments; s++ ) {

    float alpha = (float)s * segment;

    // unit vector in circle (degenerates if normal == y axis)
    math::Vector3 v1(normal.z(), 0, -normal.x());
    v1.normalize();

    math::Vector3 v2 = cross(normal, v1);

    Vertex p = center + radius * (cos(alpha) * v1 + sin(alpha) * v2);
    rays.emplace_back(p, ray.direction());
  }
  return rays;
}

}
