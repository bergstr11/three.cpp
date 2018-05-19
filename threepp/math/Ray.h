//
// Created by byter on 03.09.17.
//

#ifndef THREEPP_RAY_H
#define THREEPP_RAY_H

#include <stdexcept>
#include "Vector3.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box3.h"
#include <QDebug>

namespace three {
namespace math {

class Ray
{
  Vector3 _origin;
  Vector3 _direction;

public:
  Ray() {}

  // direction is assumed to be normalized (for accurate distance calculations)
  Ray(const Vector3 &origin, const Vector3 &direction) : _origin(origin), _direction(direction)
  {}

  Ray(const Ray &ray) : _origin(ray._origin), _direction(ray._direction)
  {}

  bool operator==(const Ray &ray)
  { return _origin == ray._origin && _direction == ray._direction; }

  Ray &set(const Vector3 &origin, const Vector3 &direction)
  {
    _origin = origin;
    _direction = direction;

    return *this;
  }

  Vector3 at(float t) const
  {
    return _direction * t + _origin;
  }

  const Vector3 &origin() const {return _origin;}
  const Vector3 &direction() const {return _direction;}

  Vector3 &origin() {return _origin;}
  Vector3 &direction() {return _direction;}

  Ray &lookAt(const Vector3 &v)
  {
    _direction = (v - _origin).normalize();

    return *this;
  }

  Ray &recast(float t)
  {
    _origin = this->at(t);

    return *this;
  }

  Vector3 closestPointToPoint(const Vector3 &point) const
  {
    Vector3 result = point - _origin;
    float directionDistance = dot(result, _direction);

    if (directionDistance < 0) {
      return _origin;
    }
    return _direction * directionDistance + _origin;
  }

  float distanceSqToPoint(const Vector3 &point) const
  {
    float directionDistance = dot((point - _origin), _direction);

    // point behind the ray
    if (directionDistance < 0) {
      return _origin.distanceToSquared(point);
    }

    Vector3 v1 = _direction * directionDistance + _origin;

    return v1.distanceToSquared(point);
  }

  float distanceToPoint(const Vector3 &point) const
  {
    return std::sqrt(distanceSqToPoint(point));
  }


  // from http://www.geometrictools.com/GTEngine/Include/Mathematics/GteDistRaySegment.h
  // It returns the min distance between the ray and the segment
  // defined by v0 and v1
  // It can also set two optional targets :
  // - The closest point on the ray
  // - The closest point on the segment
  float distanceSqToSegment(const Vector3 &v0, const Vector3 &v1,
                            Vector3 *optionalPointOnRay = nullptr, Vector3 *optionalPointOnSegment = nullptr) const
  {
    Vector3 segCenter = (v0 + v1) * 0.5;
    Vector3 segDir = (v1 - v0).normalize();
    Vector3 diff = _origin - segCenter;

    float segExtent = v0.distanceTo(v1) * 0.5f;
    float a01 = -dot(_direction, segDir);
    float b0 = dot(diff, _direction);
    float b1 = -dot(diff, segDir);
    float c = diff.lengthSq();
    float det = std::abs(1 - a01 * a01);
    float s0, s1, sqrDist, extDet;

    if (det > 0) {

      // The ray and segment are not parallel.
      s0 = a01 * b1 - b0;
      s1 = a01 * b0 - b1;
      extDet = segExtent * det;

      if (s0 >= 0) {

        if (s1 >= -extDet) {

          if (s1 <= extDet) {

            // region 0
            // Minimum at interior points of ray and segment.
            float invDet = 1 / det;
            s0 *= invDet;
            s1 *= invDet;
            sqrDist = s0 * (s0 + a01 * s1 + 2 * b0) + s1 * (a01 * s0 + s1 + 2 * b1) + c;
          }
          else {

            // region 1
            s1 = segExtent;
            s0 = std::max(0.0f, -(a01 * s1 + b0));
            sqrDist = -s0 * s0 + s1 * (s1 + 2 * b1) + c;
          }
        }
        else {

          // region 5
          s1 = -segExtent;
          s0 = std::max(0.0f, -(a01 * s1 + b0));
          sqrDist = -s0 * s0 + s1 * (s1 + 2 * b1) + c;
        }
      }
      else {

        if (s1 <= -extDet) {

          // region 4
          s0 = std::max(0.0f, -(-a01 * segExtent + b0));
          s1 = (s0 > 0) ? -segExtent : std::min(std::max(-segExtent, -b1), segExtent);
          sqrDist = -s0 * s0 + s1 * (s1 + 2 * b1) + c;
        }
        else if (s1 <= extDet) {

          // region 3
          s0 = 0;
          s1 = std::min(std::max(-segExtent, -b1), segExtent);
          sqrDist = s1 * (s1 + 2 * b1) + c;
        }
        else {

          // region 2
          s0 = std::max(0.0f, -(a01 * segExtent + b0));
          s1 = (s0 > 0) ? segExtent : std::min(std::max(-segExtent, -b1), segExtent);
          sqrDist = -s0 * s0 + s1 * (s1 + 2 * b1) + c;
        }
      }
    }
    else {

      // Ray and segment are parallel.

      s1 = (a01 > 0) ? -segExtent : segExtent;
      s0 = std::max(0.0f, -(a01 * s1 + b0));
      sqrDist = -s0 * s0 + s1 * (s1 + 2 * b1) + c;

    }

    if (optionalPointOnRay) {
      *optionalPointOnRay = _direction * s0 + _origin;
    }

    if (optionalPointOnSegment) {
      *optionalPointOnSegment = segDir * s1 + segCenter;
    }

    return sqrDist;
  }

  Vector3 intersectSphere(const Sphere &sphere)
  {
    Vector3 v1 = sphere.center() - _origin;
    float tca = dot(v1, _direction);
    float d2 = dot(v1, v1) - tca * tca;
    float radius2 = sphere.radius() * sphere.radius();

    if (d2 > radius2) throw std::range_error("d2 > radius2");

    float thc = std::sqrt(radius2 - d2);

    // t0 = first intersect point - entrance on front of sphere
    float t0 = tca - thc;

    // t1 = second intersect point - exit point on back of sphere
    float t1 = tca + thc;

    // test to see if both t0 and t1 are behind the ray - if so, return null
    if (t0 < 0 && t1 < 0) throw std::range_error("behind the ray");

    // test to see if t0 is behind the ray:
    // if it is, the ray is inside the sphere, so return the second exit point scaled by t1,
    // in order to always return an intersect point that is in front of the ray.
    if (t0 < 0) return at(t1);

    // else t0 is in front of the ray, so return the first collision point scaled by t0
    return at(t0);
  }

  bool intersectsSphere(const Sphere &sphere) const
  {
    return distanceToPoint(sphere.center()) <= sphere.radius();
  }

  bool distanceToPlane(const Plane &plane, float &distance)
  {
    float denominator = dot(plane.normal(), _direction);

    if (denominator == 0) {
      // line is coplanar, return origin
      if (plane.distanceToPoint(_origin) == 0) {
        distance = 0;
        return true;
      }

      // Null is preferable to undefined since undefined means.... it is undefined
      return false;
    }

    float t = -(dot(_origin, plane.normal()) + plane.constant()) / denominator;

    // Return if the ray never intersects the plane
    if (t >= 0) {
      distance = t;
      return true;
    }
    else
      return false;
  }

  bool intersectPlane(const Plane &plane, Vector3 &intersect)
  {
    float distance;

    if (!distanceToPlane(plane, distance)) {
      return false;
    }

    intersect = at(distance);
    return true;
  }

  bool intersectsPlane(const Plane &plane)
  {
    // check if the ray lies on the plane first
    float distToPoint = plane.distanceToPoint(_origin);

    if (distToPoint == 0)
      return true;

    float denominator = dot(plane.normal(), _direction);

    if (denominator * distToPoint < 0) {
      return true;
    }

    // ray origin is behind the plane (and is pointing behind it)
    return false;
  }

  bool intersectBox(const Box3 &box, Vector3 &intersect)
  {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    float invdirx = 1 / _direction.x(),
       invdiry = 1 / _direction.y(),
       invdirz = 1 / _direction.z();

    if (invdirx >= 0) {

      tmin = (box.min().x() - _origin.x()) * invdirx;
      tmax = (box.max().x() - _origin.x()) * invdirx;

    }
    else {

      tmin = (box.max().x() - _origin.x()) * invdirx;
      tmax = (box.min().x() - _origin.x()) * invdirx;

    }

    if (invdiry >= 0) {

      tymin = (box.min().y() - _origin.y()) * invdiry;
      tymax = (box.max().y() - _origin.y()) * invdiry;

    }
    else {

      tymin = (box.max().y() - _origin.y()) * invdiry;
      tymax = (box.min().y() - _origin.y()) * invdiry;

    }

    if ((tmin > tymax) || (tymin > tmax)) {
      return false;
    }

    // These lines also handle the case where tmin or tmax is NaN
    // (result of 0 * Infinity). x !== x returns true if x is NaN

    if (tymin > tmin || tmin != tmin) tmin = tymin;

    if (tymax < tmax || tmax != tmax) tmax = tymax;

    if (invdirz >= 0) {

      tzmin = (box.min().z() - _origin.z()) * invdirz;
      tzmax = (box.max().z() - _origin.z()) * invdirz;

    }
    else {

      tzmin = (box.max().z() - _origin.z()) * invdirz;
      tzmax = (box.min().z() - _origin.z()) * invdirz;

    }

    if ((tmin > tzmax) || (tzmin > tmax)) {
      return false;
    }

    if (tzmin > tmin || tmin != tmin) tmin = tzmin;

    if (tzmax < tmax || tmax != tmax) tmax = tzmax;

    //return point closest to the ray (positive side)

    if (tmax < 0) {
      return false;
    }

    intersect = at(tmin >= 0 ? tmin : tmax);
    return true;
  }

  bool intersectsBox(const Box3 &box)
  {
    Vector3 intersect;
    return intersectBox(box, intersect);
  }

  static void test1(bool eins) {
    Vector3 a, b, c, edge1, edge2, normal, direction, origin;

    if(eins) {
      a = {-500, 500, 0};
      b = {-500, -500, 0};
      c = {500, 500, 0};
      edge1 = b - a;
      edge2 = c - a;
      normal = cross(edge1, edge2);
      direction = {-0.378328, 0.788505, -0.484899};
      origin = {500, -1300, 800};
    }
    else {
      a = {-500, 500, 0};
      b = {-500, -500, 0};
      c = {500, 500, 0};
      edge1 = b - a;
      edge2 = c - a;
      normal = cross(edge1, edge2);
      direction = {-0.37873, 0.788029, -0.485359};
      origin = {500, -1300, 800};
    }

    float DdN = dot(direction, normal);
    float sign;

    if (DdN > 0) {

      sign = 1;
    }
    else if (DdN < 0) {

      sign = -1;
      DdN = abs(DdN);
    }
    else {
      return;
    }

    Vector3 diff = origin - a;
    float DdQxE2 = sign * dot(direction, cross(diff, edge2));

    // b1 < 0, no intersection
    if (DdQxE2 < 0) {
      return;
    }

    float DdE1xQ = sign * dot(direction, cross(edge1, diff));

    // b2 < 0, no intersection
    if (DdE1xQ < 0) {
      return;
    }

    // b1+b2 > 1, no intersection
    if (DdQxE2 + DdE1xQ > DdN) {
      return;
    }

    // Line intersects triangle, check if ray does.
    float QdN = -sign * dot(diff, normal);

    // t < 0, no intersection
    if (QdN < 0) {
      return;
    }

    float t = QdN / DdN;
    // Ray intersects triangle.
    Vector3 point = direction * t + origin;
  }

  // from http://www.geometrictools.com/GTEngine/Include/Mathematics/GteIntrRay3Triangle3.h
  bool intersectTriangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, bool backfaceCulling, Vector3 &point) const
  {
    // Compute the offset origin, edges, and normal.
    Vector3 edge1 = b - a;
    Vector3 edge2 = c - a;
    Vector3 normal = cross(edge1, edge2);

    //test1(true);
    //test1(false);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    float DdN = dot(_direction, normal);
    float sign;

    if (DdN > 0) {

      if (backfaceCulling) return false;
      sign = 1;
    }
    else if (DdN < 0) {

      sign = -1;
      DdN = abs(DdN);
    }
    else {
      return false;
    }

    Vector3 diff = _origin - a;
    float DdQxE2 = sign * dot(_direction, cross(diff, edge2));

    // b1 < 0, no intersection
    if (DdQxE2 < 0) {
      return false;
    }

    float DdE1xQ = sign * dot(_direction, cross(edge1, diff));

    // b2 < 0, no intersection
    if (DdE1xQ < 0) {
      return false;
    }

    // b1+b2 > 1, no intersection
    if (DdQxE2 + DdE1xQ > DdN) {
      return false;
    }

    // Line intersects triangle, check if ray does.
    float QdN = -sign * dot(diff, normal);

    // t < 0, no intersection
    if (QdN < 0) {
      return false;
    }

    // Ray intersects triangle.
    point = at(QdN / DdN);
    return true;
  }

  Ray &apply(const Matrix4 &matrix4)
  {
    _origin.apply(matrix4);
    _direction.transformDirection(matrix4);

    return *this;
  }
};

}
};
#endif //THREEPP_RAY_H
