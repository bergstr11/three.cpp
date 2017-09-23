//
// Created by byter on 22.08.17.
//

#ifndef THREE_QT_GEOMETRY_H
#define THREE_QT_GEOMETRY_H

#include <math/Vector3.h>
#include <math/Matrix4.h>
#include <math/Sphere.h>
#include <math/Box3.h>
#include <helper/simplesignal.h>
#include "Raycaster.h"

namespace three {

class Mesh;
class Line;
class Intersection;

using Vertex = math::Vector3;

class Geometry
{
  static uint32_t id_count;

protected:
  math::Box3 _boundingBox;
  math::Sphere _boundingSphere;

  virtual Geometry &apply(const math::Matrix4 &matrix) = 0;

  Geometry() : id(id_count++) {}

public:
  const uint32_t id;

  using OnDispose = Signal<void(Geometry *)>;
  OnDispose onDispose;

  using Ptr = std::shared_ptr<Geometry>;

  const math::Box3 &boundingBox() const {return _boundingBox;}
  const math::Sphere &boundingSphere() const {return _boundingSphere;}

  virtual Geometry &computeBoundingBox() = 0;

  virtual Geometry &computeBoundingSphere() = 0;

  virtual void raycast(const Line &line,
                       const Raycaster &raycaster, const math::Ray &ray,
                       std::vector<Intersection> &intersects) = 0;

  virtual void raycast(const Mesh &mesh,
                       const Raycaster &raycaster,
                       const math::Ray &ray,
                       math::Vector3 &intersectionPoint,
                       math::Vector3 &intersectionPointWorld,
                       std::vector<Intersection> &intersects) = 0;

  virtual bool useMorphing() const = 0;

  // rotate geometry around world x-axis
  Geometry &rotateX(float angle)
  {
    apply(math::Matrix4::rotationX( angle ));

    return *this;
  }

  // rotate geometry around world y-axis
  Geometry &rotateY(float angle)
  {
    apply(math::Matrix4::rotationY( angle ));

    return *this;
  }

  // rotate geometry around world z-axis
  Geometry &rotateZ(float angle)
  {
    apply(math::Matrix4::rotationZ( angle ));

    return *this;
  }

  // translate geometry
  Geometry &translate(float x, float y, float z)
  {
    apply(math::Matrix4::translation(x, y, z));
  }

  Geometry &scale(float x, float y, float z)
  {
    apply(math::Matrix4::scaling(x, y, z));
  }

  Geometry &lookAt(const Vertex &vector)
  {
    math::Matrix4 m1( vector, {0, 0, 0}, {0, 1, 0});

    math::Quaternion q(m1);

    apply(math::Matrix4::rotation(q));
  }

  virtual void dispose() {
    onDispose.emitSignal(this);
  }
};

}
#endif //THREE_QT_GEOMETRY_H
