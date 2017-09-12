//
// Created by byter on 10.09.17.
//

#ifndef THREE_QT_FRUSTUM_H
#define THREE_QT_FRUSTUM_H

#include <array>
#include <memory>
#include "Plane.h"
#include "Box3.h"
#include "Sphere.h"

namespace three {

class Object3D;

class Sprite;

namespace math {

class Frustum
{
  std::array<Plane, 5> _planes;

public:
  Frustum() {};

  explicit Frustum(std::array<Plane, 5> planes) : _planes(std::move(planes))
  {}

  Frustum &set(const Matrix4 &m)
  {
    const float *me = m.elements();
    float me0 = me[0], me1 = me[1], me2 = me[2], me3 = me[3];
    float me4 = me[4], me5 = me[5], me6 = me[6], me7 = me[7];
    float me8 = me[8], me9 = me[9], me10 = me[10], me11 = me[11];
    float me12 = me[12], me13 = me[13], me14 = me[14], me15 = me[15];

    _planes[0].set(me3 - me0, me7 - me4, me11 - me8, me15 - me12).normalize();
    _planes[1].set(me3 + me0, me7 + me4, me11 + me8, me15 + me12).normalize();
    _planes[2].set(me3 + me1, me7 + me5, me11 + me9, me15 + me13).normalize();
    _planes[3].set(me3 - me1, me7 - me5, me11 - me9, me15 - me13).normalize();
    _planes[4].set(me3 - me2, me7 - me6, me11 - me10, me15 - me14).normalize();
    _planes[5].set(me3 + me2, me7 + me6, me11 + me10, me15 + me14).normalize();

    return *this;
  }

  bool intersectsObject(std::shared_ptr<Object3D> object);

  bool intersectsSprite(std::shared_ptr<Sprite> sprite);

  bool intersectsSphere(const Sphere &sphere)
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

  bool intersectsBox(const Box3 &box)
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

  bool containsPoint(const Vector3 &point)
  {
    for (const Plane &plane : _planes) {

      if (plane.distanceToPoint(point) < 0) {
        return false;
      }
    }

    return true;
  }
};

}
}


#endif //THREE_QT_FRUSTUM_H
