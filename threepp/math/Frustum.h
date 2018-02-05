//
// Created by byter on 10.09.17.
//

#ifndef THREEPP_FRUSTUM_H
#define THREEPP_FRUSTUM_H

#include <array>
#include <memory>
#include "Plane.h"
#include "Box3.h"
#include "Sphere.h"
#include <iostream>

namespace three {

class Object3D;

class Sprite;

namespace math {

class Frustum
{
  std::array<Plane, 6> _planes;

public:
  Frustum() {};

  explicit Frustum(std::array<Plane, 6> planes) : _planes(std::move(planes))
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

  void print(const Matrix4 &m) {

    std::cout << "matrix: " << m.elements()[0] << ":" << m.elements()[1] << ":" << m.elements()[2] << ":" << m.elements()[3] << std::endl;
    for (const Plane &plane : _planes) {
      std::cout << "frustum: " << plane.constant() << ":" << plane.normal().x() << ":" << plane.normal().y() << ":" << plane.normal().z() << std::endl;
    }
  }

  bool intersectsObject(const Object3D &object) const;

  bool intersectsSprite(const Sprite &sprite) const;

  bool intersectsSphere(const Sphere &sphere) const;

  bool intersectsBox(const Box3 &box) const;

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


#endif //THREEPP_FRUSTUM_H
