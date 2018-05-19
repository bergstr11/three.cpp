//
// Created by byter on 10.09.17.
//
#include "impl/raycast.h"

namespace three {

void intersectObject(Object3D &object,
                     const Raycaster &raycaster,
                     std::vector<Intersection> &intersects,
                     bool recursive,
                     bool stopFirst )
{
  if (!object.visible()) return;

  object.raycast( raycaster, intersects );
  if(stopFirst && !intersects.empty()) return;

  if (recursive ) {
    const std::vector<Object3D::Ptr> &children = object.children();

    for (auto child : children) {
      intersectObject(*child, raycaster, intersects, recursive);
      if(stopFirst && !intersects.empty()) return;
    }
  }
}

std::vector<Intersection> Raycaster::intersectObject(Object3D &object, bool recursive, bool stopFirst )
{
  std::vector<Intersection> intersects;

  three::intersectObject( object, *this, intersects, recursive, stopFirst);

  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance > b.distance;});

  return intersects;
}

std::vector<Intersection> Raycaster::intersectObjects(const std::vector<Object3D::Ptr> objects,
                                           bool recursive, bool stopFirst )
{
  std::vector<Intersection> intersects;

  for (auto obj : objects) {
    three::intersectObject( *obj, *this, intersects, recursive );
    if(stopFirst && !intersects.empty()) break;
  }
  std::sort(intersects.begin(), intersects.end(),
            [](const Intersection &a, const Intersection &b) {return a.distance < b.distance;});

  return intersects;
}

std::vector<math::Ray> Raycaster::createCircularBundle(
   const math::Ray &ray, float radius, unsigned radialSegments)
{
  std::vector<math::Ray> rays;
  for (unsigned s = 1; s <= radialSegments; s ++ ) {

    float segment = s / radialSegments * 2.0f * (float)M_PI;

    const auto &c = ray.origin();
    const auto &v = ray.direction();

    /*
     * a ⋅ v = 0; b = a × v
     *
     * x(θ) = c1 + r cos(θ) a1 + r sin(θ) b1
     * y(θ) = c2 + r cos(θ) a2 + r sin(θ) b2
     * z(θ) = c3 + r cos(θ) a3 + r sin(θ) b3
     */
    Vertex a = v.normalized();
    Vertex b = cross(a, v);
    float rcos = radius * (float)cos(segment), rsin = radius * (float)sin(segment);

    float x = c.x() + rcos * a.x() + rsin * b.x();
    float y = c.y() + rcos * a.y() + rsin * b.y();
    float z = c.z() + rcos * a.z() + rsin * b.z();

    rays.emplace_back(Vertex(x, y, z), ray.direction());
  }
  return rays;
}

std::vector<math::Ray> Raycaster::createSquareBundle(
   const math::Ray &ray, float sideLength, unsigned sideSegments)
{
  std::vector<math::Ray> rays;
  return rays;
}

}
