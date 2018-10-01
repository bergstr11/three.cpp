//
// Created by byter on 04.09.17.
//

#ifndef THREEPP_RAYCASTER_H
#define THREEPP_RAYCASTER_H

#include <memory>

#include <threepp/util/osdecl.h>
#include <threepp/math/Ray.h>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>
#include "Face3.h"

#ifdef near
#undef near
#undef far
#endif

namespace three {

class Raycaster;
class Object3D;

/**
 * describes a hit point of a ray
 */
struct Intersection
{
  float distance;
  math::Vector3 direction;
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

/**
 * holds the intersections for each ray in a ray bundle
 */
class DLX IntersectList
{
  std::vector<std::vector<Intersection>> _intersections;

  class DLX iterator : public std::iterator<std::output_iterator_tag, int>
  {
  public:
    iterator(IntersectList &list, bool end);

    Intersection &operator*() const;
    iterator & operator++();
    iterator & operator++(int);
    bool operator ==(const iterator &) const;
    bool operator !=(const iterator &) const;

  private:
    size_t _index;
    std::vector<Intersection>::iterator _begin;
    bool _end;
    IntersectList &_list;
  };

public:
  IntersectList(const IntersectList &is) = delete;
  IntersectList(const IntersectList &&is) = delete;

  IntersectList() = default;

  iterator begin();
  iterator end();

  void prepare();

  unsigned rayCount() const {return _intersections.size();}

  /**
   * calculate the surface to place a planar, circular object of the same
   * diameter as the ray bundle
   *
   * @param position (out) the center point of the surface
   * @param normal (out) the normal of the surface
   * @return the object owning the surface
   */
  Object3D *calculateSurface(math::Vector3 &position, math::Vector3 &normal);

  Intersection &add(unsigned rayIndex, const Intersection &intersection)
  {
    if(rayIndex >= _intersections.size()) _intersections.resize(rayIndex+1);
    _intersections[rayIndex].push_back(intersection);
    return _intersections[rayIndex].back();
  }

  Intersection &add(unsigned rayIndex)
  {
    if(rayIndex >= _intersections.size()) _intersections.resize(rayIndex+1);
    _intersections[rayIndex].emplace_back();
    return _intersections[rayIndex].back();
  }

  Intersection &get(unsigned rayIndex, unsigned intersectIndex)
  {
    return _intersections[rayIndex][intersectIndex];
  }

  bool hasIntersects(float distance)
  {
    for(const auto &intersects : _intersections) {
      for (const auto &intersect : intersects) {
        if (intersect.distance <= distance) return true;
      }
    }
    return false;
  }

  void clear() {
    _intersections.clear();
  }

  bool empty() const {
    for(const auto &intersects : _intersections) {
      if(!intersects.empty()) return false;
    }
    return true;
  }
};

/**
 * casts one or more rays, collecting objects along the path
 */
class DLX Raycaster
{
  std::vector<math::Ray> _rays;
  float _near=0, _far=std::numeric_limits<float>::infinity();

  float _linePrecision = 1;

  math::Vector3 _origin;

  static std::vector<math::Ray> createCircularBundle(const math::Ray &ray,
                                                     float radius,
                                                     unsigned radialSegments);

public:
  /**
   * create a raycaster with a circular bundle of rays
   *
   * @param cameraRay the ray around which the ray bundle will be arranged
   * @param radius the radius of the circle on which the rays are placed
   * @param radialSegments the number of rays to place
   */
  static Raycaster circular(const math::Ray &cameraRay, float radius, unsigned radialSegments)
  {
    return Raycaster(cameraRay.origin(), Raycaster::createCircularBundle(cameraRay, radius, radialSegments));
  }

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

  const math::Ray &centerRay() const {return _rays[0];}

  float linePrecision() const {return _linePrecision;}

  const std::vector<math::Ray> &rays() const {return _rays;}

  float near() const {return _near;}
  float far() const {return _far;}
  const math::Vector3 &origin() const {return _origin;}

  void intersectObject(Object3D &object,
                       IntersectList &intersects,
                       bool recursive = true) const;

  void intersectObjects(std::vector<std::shared_ptr<Object3D>> objects,
                        IntersectList &intersects,
                        bool recursive=true) const;
};

}
#endif //THREEPP_RAYCASTER_H
