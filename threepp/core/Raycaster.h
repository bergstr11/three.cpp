//
// Created by byter on 04.09.17.
//

#ifndef THREEPP_RAYCASTER_H
#define THREEPP_RAYCASTER_H

#include <memory>

#include <threepp/math/Ray.h>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>
#include "Face3.h"

namespace three {

class Raycaster;
class Object3D;

struct Intersection
{
  float distance;
  math::Vector3 point;
  math::Vector2 uv;
  Object3D *object;
  Face3 face;

  /**
   * triangle number in indices buffer (BufferGeometry only)
   */
  unsigned faceIndex;

  /**
   * triangle number in positions/vertices buffer (LinearGeometry and BufferGeometry)
   */
  unsigned index;
};

void intersectObject(Object3D &object,
                     const Raycaster &raycaster,
                     std::vector<Intersection> &intersects,
                     bool recursive = true,
                     bool stopFirst = false);

/**
 * casts one ray, collecting objects along the path
 */
class Raycaster
{
  std::vector<math::Ray> _rays;
  float _near=0, _far=std::numeric_limits<float>::infinity();

  float _linePrecision = 1;

  math::Vector3 _origin;

public:
  /**
   * create a circular bundle of rays
   *
   * @param cameraRay the ray around which the ray bundle will be arranged
   * @param radius the radius of the circle on which the rays are placed
   * @param radialSegments the number of rays to place
   */
  static std::vector<math::Ray> createCircularBundle(const math::Ray &ray,
                                                     float radius,
                                                     unsigned radialSegments);

  /**
   * create a square bundle of rays
   *
   * @param cameraRay the ray around which the ray bundle will be arranged
   * @param sideLength the side length of the square along which the rays are placed
   * @param sideSegments the number of rays to place on one side
   */
  static std::vector<math::Ray> createSquareBundle(const math::Ray & ray,
                                                   float sideLength,
                                                   unsigned sideSegments);

  /**
   * create a ray bundle caster
   *
   * @param origin the origin at the center of the bundle
   * @param rays the bundle rays
   */
  Raycaster(const math::Vector3 &origin, const std::vector<math::Ray> rays)
     : _origin(origin), _rays(rays) {}

  Raycaster() = default;

  /**
   * create a single ray caster
   *
   * @param ray the single ray
   */
  Raycaster(const math::Ray &ray) : _origin(ray.origin()) {
    _rays.push_back(ray);
  }

  /**
   * configure a single ray caster
   *
   * @param ray the ray to to cast
   * @return this object
   */
  Raycaster &set(const math::Ray &ray )
  {
    _rays.clear();
    _rays.push_back(ray);
    _origin = ray.origin();
    return *this;
  }

  float linePrecision() const {return _linePrecision;}
  const std::vector<math::Ray> &rays() const {return _rays;}

  const float near() const {return _near;}
  const float far() const {return _far;}
  const math::Vector3 &origin() const {return _origin;}

  std::vector<Intersection> intersectObject(Object3D &object, bool recursive=true, bool stopFirst=true );

  std::vector<Intersection> intersectObjects(std::vector<std::shared_ptr<Object3D>> objects,
                                             bool recursive=true, bool stopfirst=true);
};

}
#endif //THREEPP_RAYCASTER_H
