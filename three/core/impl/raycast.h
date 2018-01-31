//
// Created by byter on 08.09.17.
//

#ifndef THREE_QT_RAYCAST_H
#define THREE_QT_RAYCAST_H

#include <math/Triangle.h>
#include <core/Object3D.h>
#include <core/Raycaster.h>
#include <core/Face3.h>
#include <material/Material.h>

namespace three {
namespace impl {

inline bool checkIntersection(const Object3D &object,
                              const Material::Ptr &material,
                              const Raycaster &raycaster,
                              const math::Ray &ray,
                              const math::Vector3 &pA, const math::Vector3 &pB, const math::Vector3 &pC,
                              Intersection &result)
{
  bool intersect;
  if (material->side == Side::Back) {
    intersect = ray.intersectTriangle(pC, pB, pA, true, result.point);
  }
  else {
    intersect = ray.intersectTriangle(pA, pB, pC, material->side != Side::Double, result.point);
  }

  if (!intersect) return false;

  result.point.apply(object.matrixWorld());
  const math::Matrix4 &m = object.matrixWorld();

  float distance = raycaster.ray().origin().distanceTo(result.point);

  if (distance < raycaster.near() || distance > raycaster.far()) return false;

  result.distance = distance;
  result.object = &object;

  return true;
}

inline math::Vector2 uvIntersection(const math::Vector3 &point,
                                    const math::Vector3 &p1,
                                    const math::Vector3 &p2,
                                    const math::Vector3 &p3,
                                    math::Vector2 &uv1,
                                    math::Vector2 &uv2,
                                    math::Vector2 &uv3)
{
  math::Vector3 barycoord = math::Triangle::barycoordFromPoint(point, p1, p2, p3);

  return (uv1 * barycoord.x()) + (uv2 * barycoord.y()) + (uv3 * barycoord.z());
}

inline bool checkBufferGeometryIntersection(const Object3D &object,
                                            const Raycaster &raycaster,
                                            const math::Ray &ray,
                                            const BufferAttributeT<float>::Ptr &position,
                                            const BufferAttributeT<float>::Ptr &uv,
                                            unsigned a, unsigned b, unsigned c,
                                            Intersection &intersection)
{

  math::Vector3 vA = math::Vector3::fromBufferAttribute(*position, a);
  math::Vector3 vB = math::Vector3::fromBufferAttribute(*position, b);
  math::Vector3 vC = math::Vector3::fromBufferAttribute(*position, c);

  if (checkIntersection(object, object.material(), raycaster, ray, vA, vB, vC, intersection)) {

    math::Vector2 uvA = math::Vector2::fromBufferAttribute(*uv, a);
    math::Vector2 uvB = math::Vector2::fromBufferAttribute(*uv, b);
    math::Vector2 uvC = math::Vector2::fromBufferAttribute(*uv, c);

    intersection.uv = uvIntersection(intersection.point, vA, vB, vC, uvA, uvB, uvC);

    intersection.face = Face3(a, b, c, math::Triangle::normal(vA, vB, vC));
    intersection.faceIndex = a;

    return true;
  }

  return false;
}

}
}
#endif //THREE_QT_RAYCAST_H
