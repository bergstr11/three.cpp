//
// Created by byter on 08.09.17.
//

#ifndef THREEPP_RAYCAST_H
#define THREEPP_RAYCAST_H

#include <threepp/math/Triangle.h>
#include <threepp/core/Object3D.h>
#include <threepp/core/Raycaster.h>
#include <threepp/core/Face3.h>
#include <threepp/core/BufferAttribute.h>
#include <threepp/material/Material.h>

#ifdef near
#undef near
#undef far
#endif

namespace three {
namespace impl {

inline bool checkIntersection(const Object3D &object,
                              const Material &material,
                              const Raycaster &raycaster,
                              const math::Ray &ray,
                              const math::Vector3 &pA, const math::Vector3 &pB, const math::Vector3 &pC,
                              Intersection &result)
{
  bool intersect;
  if (material.side == Side::Back) {
    intersect = ray.intersectTriangle(pC, pB, pA, true, result.point);
    if(intersect) result.front = false;
  }
  else {
    intersect = ray.intersectTriangle(pA, pB, pC, material.side != Side::Double, result.point);
    if(intersect) result.front = true;
    else {
      intersect = ray.intersectTriangle(pC, pB, pA, true, result.point);
      if(intersect) result.front = false;
    }
  }

  if (!intersect) return false;

  result.point.apply(object.matrixWorld());

  float distance = raycaster.origin().distanceTo(result.point);

  if (distance < raycaster.near() || distance > raycaster.far()) return false;

  result.distance = distance;
  result.object = &const_cast<Object3D &>(object);

  return true;
}

inline math::Vector2 uvIntersection(const math::Vector3 &point,
                                    const math::Vector3 &p1,
                                    const math::Vector3 &p2,
                                    const math::Vector3 &p3,
                                    const math::Vector2 &uv1,
                                    const math::Vector2 &uv2,
                                    const math::Vector2 &uv3)
{
  math::Vector3 barycoord = math::Triangle::barycoordFromPoint(point, p1, p2, p3);

  return (uv1 * barycoord.x()) + (uv2 * barycoord.y()) + (uv3 * barycoord.z());
}

inline bool checkBufferGeometryIntersection(const Object3D &object,
                                            const Material &material,
                                            const Raycaster &raycaster,
                                            const math::Ray &ray,
                                            const BufferAttributeT<float>::Ptr &position,
                                            const BufferAttributeT<float>::Ptr &uv,
                                            unsigned a, unsigned b, unsigned c,
                                            Intersection &intersection)
{
  const math::Vector3 &vA = position->item_at<math::Vector3>(a);
  const math::Vector3 &vB = position->item_at<math::Vector3>(b);
  const math::Vector3 &vC = position->item_at<math::Vector3>(c);

  if (checkIntersection(object, material, raycaster, ray, vA, vB, vC, intersection)) {

    if(uv) {
      const math::Vector2 &uvA = uv->item_at<math::Vector2>(a);
      const math::Vector2 &uvB = uv->item_at<math::Vector2>(b);
      const math::Vector2 &uvC = uv->item_at<math::Vector2>(c);

      intersection.uv = uvIntersection(intersection.point, vA, vB, vC, uvA, uvB, uvC);
    }

    intersection.face = Face3(a, b, c, math::Triangle::normal(vA, vB, vC));
    intersection.faceIndex = a;

    return true;
  }

  return false;
}

}
}
#endif //THREEPP_RAYCAST_H
