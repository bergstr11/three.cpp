//
// Created by byter on 08.09.17.
//

#ifndef THREE_QT_RAYCAST_H
#define THREE_QT_RAYCAST_H

#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Triangle.h>
#include <core/Object3D.h>
#include <core/Raycaster.h>
#include <core/Face3.h>
#include <material/Material.h>

namespace three {

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

namespace impl {

inline bool checkIntersection(const Object3D &object,
                              const Material::Ptr &material,
                              const Raycaster &raycaster,
                              const math::Ray &ray,
                              const math::Vector3 &pA, const math::Vector3 &pB, const math::Vector3 &pC,
                              math::Vector3 &point,
                              Intersection &result)
{
  bool intersect;
  if (material->_side == Side::Back) {
    intersect = ray.intersectTriangle(pC, pB, pA, true, point);
  }
  else {
    intersect = ray.intersectTriangle(pA, pB, pC, material->_side != Side::Double, point);
  }

  if (!intersect) return false;

  math::Vector3 intersectionPointWorld = point;
  intersectionPointWorld.apply(object.matrixWorld());

  float distance = raycaster.ray().origin().distanceTo(intersectionPointWorld);

  if (distance < raycaster.near() || distance > raycaster.far()) return false;

  result.distance = distance;
  result.point = intersectionPointWorld;
  result.object = &object;

  return true;
}

inline math::Vector2 uvIntersection(const math::Vector3 &point, const math::Vector3 &p1, const math::Vector3 &p2,
                                    const math::Vector3 &p3, math::Vector2 &uv1, math::Vector2 &uv2, math::Vector2 &uv3)
{
  math::Vector3 barycoord = math::Triangle::barycoordFromPoint(point, p1, p2, p3);

  return (uv1 * barycoord.x()) + (uv2 * barycoord.y()) + (uv3 * barycoord.z());
}

inline bool checkBufferGeometryIntersection(const Object3D &object,
                                            const Raycaster &raycaster,
                                            const math::Ray &ray,
                                            const BufferAttribute<float>::Ptr &position,
                                            const BufferAttribute<float>::Ptr &uv,
                                            unsigned a, unsigned b, unsigned c,
                                            math::Vector3 &intersectionPoint,
                                            Intersection &intersection)
{

  math::Vector3 vA = math::Vector3::fromBufferAttribute(*position, a);
  math::Vector3 vB = math::Vector3::fromBufferAttribute(*position, b);
  math::Vector3 vC = math::Vector3::fromBufferAttribute(*position, c);

  if (checkIntersection(object, object.material(), raycaster, ray, vA, vB, vC, intersectionPoint, intersection)) {

    math::Vector2 uvA = math::Vector2::fromBufferAttribute(*uv, a);
    math::Vector2 uvB = math::Vector2::fromBufferAttribute(*uv, b);
    math::Vector2 uvC = math::Vector2::fromBufferAttribute(*uv, c);

    intersection.uv = uvIntersection(intersectionPoint, vA, vB, vC, uvA, uvB, uvC);

    intersection.face = Face3(a, b, c, math::Triangle::normal(vA, vB, vC));
    intersection.faceIndex = a;

    return true;
  }

  return false;
}

}
}
#endif //THREE_QT_RAYCAST_H
