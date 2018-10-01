//
// Created by byter on 12.08.17.
//

#ifndef THREEPP_BOX3_H
#define THREEPP_BOX3_H

#include "Vector3.h"
#include <vector>

namespace three {
namespace math {

class Sphere;
class Plane;

class DLX Box3
{
  using flt = std::numeric_limits<float>;
  
  Vector3 _min;
  Vector3 _max;
  
public:
  Box3(const Vector3 &min, const Vector3 &max) : _min(min), _max(max) {}
  Box3(const Box3 &box) : _min(box._min), _max(box._max) {}
  Box3() : _min({flt::infinity()}), _max({-flt::infinity()}) {}

  const Vector3 &min() const {return _min;}
  const Vector3 &max() const {return _max;}

  static Box3 fromArray(float *array, unsigned length) 
  {
    float minX = + flt::infinity();
    float minY = + flt::infinity();
    float minZ = + flt::infinity();

    float maxX = - flt::infinity();
    float maxY = - flt::infinity();
    float maxZ = - flt::infinity();

    for(unsigned i = 0; i < length; i += 3 ) {

      float x = array[ i ];
      float y = array[ i + 1 ];
      float z = array[ i + 2 ];

      if ( x < minX ) minX = x;
      if ( y < minY ) minY = y;
      if ( z < minZ ) minZ = z;

      if ( x > maxX ) maxX = x;
      if ( y > maxY ) maxY = y;
      if ( z > maxZ ) maxZ = z;
    }

    return Box3(Vector3( minX, minY, minZ ), Vector3( maxX, maxY, maxZ ));
  }

  Box3 &set(const std::vector<Vector3> points)
  {
    for(const Vector3 &point : points)
    {
      expandByPoint(point);
    }
    return *this;
  }

  static Box3 fromPoints(const Vector3 *points, size_t length)
  {
    Box3 box;

    for(size_t i=0; i<length; i++)
    {
      box.expandByPoint(points[i]);
    }

    return box;
  }

  static Box3 fromCenterAndSize(const Vector3 &center, const Vector3 &size)
  {
    Vector3 halfSize = size * 0.5;
    return Box3(center - halfSize, center + halfSize);
  }

  void makeEmpty()
  {
    _min.x() = _min.y() = _min.z() = + flt::infinity();
    _max.x() = _max.y() = _max.z() = - flt::infinity();
  }

  bool isEmpty() const
  {
    // this is a more robust check for empty than ( volume <= 0 ) because volume can get positive with two negative axes
    return _max.x() < _min.x() || _max.y() < _min.y() || _max.z() < _min.z();
  }

  Vector3 getCenter() const
  {
    return isEmpty() ? Vector3( 0, 0, 0 ) : (_min + _max) * 0.5;
  }

  Vector3 getSize() const
  {
    return isEmpty() ? Vector3( 0, 0, 0 ) : _max - _min;
  }

  Box3 &expandByPoint(const Vector3 &point)
  {
    _min.min( point );
    _max.max( point );

    return *this;
  }

  Box3 &expandByVector(const Vector3 &vector)
  {
    _min -= vector;
    _max += vector;

    return *this;
  }

  Box3 &expandByScalar(float scalar)
  {
    _min += -scalar;
    _max += scalar;

    return *this;
  }

  bool containsPoint(const Vector3 &point) const
  {

    return !(point.x() < _min.x() || point.x() > _max.x() ||
             point.y() < _min.y() || point.y() > _max.y() ||
             point.z() < _min.z() || point.z() > _max.z());
  }

  bool containsBox(const Box3 &box) const
  {
    return _min.x() <= box._min.x() && box._max.x() <= _max.x() &&
           _min.y() <= box._min.y() && box._max.y() <= _max.y() &&
           _min.z() <= box._min.z() && box._max.z() <= _max.z();
  }

  Vector3 getParameter(const Vector3 &point) const
  {
    // This can potentially have a divide by zero if the box
    // has a size dimension of 0.
    return Vector3(
       (point.x() - _min.x()) / (_max.x() - _min.x()),
       (point.y() - _min.y()) / (_max.y() - _min.y()),
       (point.z() - _min.z()) / (_max.z() - _min.z())
    );
  }

  bool intersectsBox(const Box3 &box) const
  {
    // using 6 splitting planes to rule out intersections.
    return !(box._max.x() < _min.x() || box._min.x() > _max.x() ||
             box._max.y() < _min.y() || box._min.y() > _max.y() ||
             box._max.z() < _min.z() || box._min.z() > _max.z());
  }

  bool intersectsSphere(const Sphere &sphere) const;

  bool intersectsPlane(const Plane &plane ) const;

  Vector3 clampPoint(const Vector3 &point) const
  {
    return Vector3( point ).clamp( _min, _max );
  }

  float distanceToPoint(const Vector3 &point) const
  {
    Vector3 clampedPoint {point};
    clampedPoint.clamp(_min, _max);
    return (clampedPoint - point).length();
  }

  Sphere getBoundingSphere() const;

  Box3 &intersect(const Box3 &box)
  {
    _min.max( box._min );
    _max.min( box._max );

    // ensure that if there is no overlap, the result is fully empty, not slightly empty with non-inf/+inf values that will cause subsequence intersects to erroneously return valid values.
    if(isEmpty()) makeEmpty();

    return *this;
  }

  Box3 &unify(const Box3 &box )
  {
     _min.min( box._min );
     _max.max( box._max );

     return *this;
  }

  Box3 operator *(const Matrix4 &matrix)
  {
    // transform of empty box is an empty box.
    if( isEmpty() ) return *this;

    Vector3 points[] {
       {_min.x(), _min.y(), _min.z()}, //000
       {_min.x(), _min.y(), _max.z()}, //001
       {_min.x(), _max.y(), _min.z()}, //010
       {_min.x(), _max.y(), _max.z()}, //011
       {_max.x(), _min.y(), _min.z()}, //100
       {_max.x(), _min.y(), _max.z()}, //101
       {_max.x(), _max.y(), _min.z()}, //110
       {_max.x(), _max.y(), _max.z()}  //111
    };
    for(int i=0; i<8; i++) {
      points[i].apply(matrix);
    }

    return Box3::fromPoints(points, 8);
  }

  Box3 &translate(const Vector3 &offset)
  {
    _min += offset;
    _max += offset;

    return *this;
  }

  bool operator == (const Box3 &box ) const
  {
    return box._min == _min && box._max == _max;
  }
};

}
}


#endif //THREEPP_BOX3_H
