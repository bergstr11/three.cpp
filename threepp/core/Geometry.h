//
// Created by byter on 22.08.17.
//

#ifndef THREEPP_GEOMETRY_H
#define THREEPP_GEOMETRY_H

#include <threepp/math/Vector3.h>
#include <threepp/math/Matrix4.h>
#include <threepp/math/Sphere.h>
#include <threepp/math/Box3.h>
#include <threepp/util/simplesignal.h>
#include <threepp/util/Types.h>
#include "Raycaster.h"

namespace three {

class Mesh;
class Line;
class Intersection;

class BufferGeometry;

class Geometry
{
  friend class BufferGeometry;

  static size_t id_count;

protected:
  math::Box3 _boundingBox;
  math::Sphere _boundingSphere;

  std::vector<Group> _groups;

  virtual Geometry &apply(const math::Matrix4 &matrix) = 0;

  Geometry() : id(id_count++) {}

  Geometry(const Geometry &geometry) : id(id_count++)
  {
    _boundingBox = geometry._boundingBox;
    _boundingSphere = geometry._boundingSphere;
    _groups = geometry._groups;
  }

public:
  virtual ~Geometry() {}

  const size_t id;

  using OnDispose = Signal<void(Geometry *)>;
  OnDispose onDispose;

  using Ptr = std::shared_ptr<Geometry>;

  const math::Box3 &boundingBox() const {return _boundingBox;}
  math::Box3 &boundingBox() {return _boundingBox;}
  const math::Sphere &boundingSphere() const {return _boundingSphere;}
  math::Sphere &boundingSphere() {return _boundingSphere;}

  virtual Geometry &computeBoundingBox() = 0;

  virtual Geometry &computeBoundingSphere() = 0;

  virtual void raycast(const Line &line,
                       const Raycaster &raycaster, const math::Ray &ray,
                       std::vector<Intersection> &intersects) {}

  virtual void raycast(const Mesh &mesh,
                       const Raycaster &raycaster,
                       const math::Ray &ray,
                       std::vector<Intersection> &intersects) {}

  virtual bool useMorphing() const = 0;

  virtual size_t vertexCount() const = 0;

  Geometry &addGroup(uint32_t start, uint32_t count, uint32_t materialIndex=0)
  {
    _groups.emplace_back(start, count, materialIndex);
  }

  Geometry &clearGroups()
  {
    _groups.clear();
  }

  const std::vector<Group> &groups() const {return _groups;}

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

  virtual Geometry *cloned() const = 0;

  virtual void dispose() {
    onDispose.emitSignal(this);
  }
};

}
#endif //THREEPP_GEOMETRY_H
