//
// Created by byter on 04.09.17.
//

#ifndef THREE_QT_RAYCASTER_H
#define THREE_QT_RAYCASTER_H

#include <math/Ray.h>
#include <memory>

namespace three {

class Raycaster;
class Object3D;
class Intersection;

void intersectObject(const Object3D &object, const Raycaster &raycaster, std::vector<Intersection> &intersects, bool recursive);

class Raycaster
{
  math::Ray _ray;
  float _near, _far;

  float _linePrecision = 1;

public:
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
  const float near() const {return _near;}
  const float far() const {return _far;}

  std::vector<Intersection> intersectObject(const Object3D &object, bool recursive );

  std::vector<Intersection> intersectObjects(std::vector<std::shared_ptr<Object3D>> objects,
                                             bool recursive );
};

}
#endif //THREE_QT_RAYCASTER_H
