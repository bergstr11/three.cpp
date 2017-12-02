//
// Created by byter on 18.08.17.
//

#ifndef THREE_MATH_PLANE_H
#define THREE_MATH_PLANE_H

#include "Vector3.h"
#include "Sphere.h"
#include "Box3.h"
#include "Line3.h"

namespace three {
namespace math {

class Plane
{
  Vector3 _normal;
  float _constant;

public:
  Plane() : _normal(1.0f, 0.0f, 0.0f), _constant(0) {}
  Plane(const Vector3 &normal, const float constant) : _normal(normal), _constant(constant) {}
  Plane(const Plane &plane) : _normal(plane._normal), _constant(plane._constant) {}

  const Vector3 &normal() const {return _normal;}
  const float constant() const {return _constant;}

  Plane &set(const Vector3 &normal, float constant)
  {
    _normal = normal;
    _constant = constant;

    return *this;
  }

  Plane &set(float x, float y, float z, float w )
  {
    _normal.set( x, y, z );
    _constant = w;

    return *this;
  }

  Plane &setNormalAndCoplanarPoint(const Vector3 &normal, const Vector3 &point)
  {
    _normal = normal;
    _constant = - point.dot( _normal );

    return *this;
  }

  Plane &setCoplanarPoints(const Vector3 &a, const Vector3 &b, const Vector3 &c)
  {
    Vector3 normal = ((c - b).cross(a - b)).normalize();

    // Q: should an error be thrown if normal is zero (e.g. degenerate plane)?
    setNormalAndCoplanarPoint( normal, a );

    return *this;
  }

  Plane &normalize()
  {
    // Note: will lead to a divide by zero if the plane is invalid.

    float inverseNormalLength = 1.0f / _normal.length();
    _normal *= inverseNormalLength;
    _constant *= inverseNormalLength;

    return *this;
  }

  Plane &negate()
  {
    _constant *= - 1;
    _normal.negate();

    return *this;
  }

  float distanceToPoint(const Vector3 &point ) const
  {
     return _normal.dot( point ) + _constant;
  }

  float distanceToSphere(const Sphere &sphere) const
  {
     return distanceToPoint( sphere.center() ) - sphere.radius();
  }

  Vector3 projectPoint(const Vector3 &point) const
  {
    Vector3 result {_normal};
    result *= (-distanceToPoint( point ));
    return result + point;
  }

  bool intersectLine(const Line3 &line, Vector3 &intersect)
  {
    Vector3 direction = line.delta();

    float denominator = _normal.dot( direction );

    if ( denominator == 0 ) {

      // line is coplanar, return origin
      if (distanceToPoint(line.start()) == 0 ) {

        intersect = line.start();
        return true;
      }

      return false;

    }

    float t = - ( line.start().dot( _normal ) + _constant ) / denominator;

    if ( t < 0 || t > 1 ) return false;

    intersect = direction * t + line.start();
    return true;
  }

  // Note: this tests if a line intersects the plane, not whether it (or its end-points) are coplanar with it.
  bool intersectsLine(const Line3 &line)
  {
     float startSign = distanceToPoint(line.start());
     float endSign = distanceToPoint(line.end());

     return ( startSign < 0 && endSign > 0 ) || ( endSign < 0 && startSign > 0 );
  }

  bool intersectsBox(const Box3 &box)
  {
     return box.intersectsPlane( *this );
  }

  bool intersectsSphere(const Sphere &sphere)
  {
     return sphere.intersectsPlane(*this);
  }

  Vector3 coplanarPoint()
  {
     return Vector3( _normal ) * -_constant;
  }

  Plane &apply(const Matrix4 &matrix)
  {
    Matrix3 normalMatrix = matrix.normalMatrix();

    Vector3 referencePoint = coplanarPoint() * matrix;

    _normal.apply(normalMatrix);
    _normal.normalize();

    _constant = - referencePoint.dot( _normal );

    return *this;
  }

  Plane &translate(const Vector3 &offset )
  {
     _constant -= offset.dot( _normal );

     return *this;
  }

  bool operator ==(const Plane &plane) const
  {
     return plane._normal == _normal && plane._constant == _constant;
  }
};

}
}
#endif //THREE_MATH_PLANE_H
