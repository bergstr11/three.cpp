//
// Created by byter on 04.09.17.
//

#ifndef THREE_QT_RAYCASTER_H
#define THREE_QT_RAYCASTER_H

#include <memory>

#include <three/math/Ray.h>
#include <three/math/Vector2.h>
#include <three/math/Vector3.h>
#include "Face3.h"

namespace three {

class Raycaster;
class Object3D;

struct Intersection
{
  float distance;
  math::Vector3 point;
  math::Vector2 uv;
  const Object3D *object;
  Face3 face;
  unsigned faceIndex;
  unsigned index;
};

void intersectObject(const Object3D &object, const Raycaster &raycaster, std::vector<Intersection> &intersects, bool recursive);

class Raycaster
{
  math::Ray _ray;
  float _near=0, _far=std::numeric_limits<float>::infinity();

  float _linePrecision = 1;

public:
  Raycaster() = default;

  // direction is assumed to be normalized (for accurate distance calculations)
  Raycaster(const math::Vector3 &origin, const math::Vector3 &direction, float near=0, float far=std::numeric_limits<float>::infinity())
     : _ray(origin, direction), _near(near), _far(far) {}

  Raycaster &set(const math::Vector3 &origin, const math::Vector3 &direction )
  {
    // direction is assumed to be normalized (for accurate distance calculations)
    _ray.set( origin, direction );
  }

  float linePrecision() const {return _linePrecision;}
  const math::Ray &ray() const {return _ray;}
  math::Ray &ray() {return _ray;}
  const float near() const {return _near;}
  const float far() const {return _far;}

  std::vector<Intersection> intersectObject(const Object3D &object, bool recursive );

  std::vector<Intersection> intersectObjects(std::vector<std::shared_ptr<Object3D>> objects,
                                             bool recursive=false);
};

}
#endif //THREE_QT_RAYCASTER_H
