//
// Created by byter on 11.08.17.
//

#ifndef THREE_QT_BOX2_H
#define THREE_QT_BOX2_H

#include "Vector2.h"
#include <vector>

namespace three {
namespace math {

class Box2
{
  Vector2 _min, _max;

public:
  Box2(const Vector2 &min, const Vector2 &max) : _min(min), _max(max) {}

  Box2(const Box2 &other) : _min(other._min), _max(other._max) {}

  Box2(float minScalar, float maxScalar) : _min(Vector2(minScalar)), _max(Vector2(maxScalar)) {}

  static Box2 empty()
  {
    return Box2(+std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
  }

  static Box2 fromPoints(std::vector<Vector2> points)
  {
    Box2 box(+std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

    for (const Vector2 &point : points) {
      box.expand(point);
    }
    return box;
  }

  static Box2 fromCenterAndSize(const Vector2 &center, const Vector2 &size)
  {
    Vector2 halfSize = size * 0.5;
    return Box2(center - halfSize, center + halfSize);
  }

  Box2 &set(const Box2 &box)
  {
    _min = box._min;
    _max = box._max;

    return *this;
  }

  Box2 &expand(const Vector2 &vector )
  {
    _min -= vector;
    _max += vector;

    return *this;
  }

  Box2 &expand(float scalar)
  {
    _min += -scalar;
    _max += scalar;

    return *this;
  }

  bool isEmpty()
  {
    // this is a more robust check for empty than ( volume <= 0 ) because volume can get positive with two negative axes
    return (_max.x() < _min.x() ) || ( _max.y() < _min.y() );
  }

  Vector2 getCenter()
  {
    return isEmpty() ? Vector2(0.0, 0.0) : Vector2(_min, _max) * 0.5;
  }

  Vector2 getSize()
  {
    return isEmpty() ? Vector2(0.0, 0.0) : _max - _min;
  }

  bool containsPoint(Vector2 point)
  {
    return !(point.x() < _min.x() || point.x() > _max.x() || point.y() < _min.y() || point.y() > _max.y());
  }

  bool containsBox(const Box2 box)
  {
    return _min.x() <= box._min.x() && box._max.x() <= _max.x() &&
           _min.y() <= box._min.y() && box._max.y() <= _max.y();
  }

  Vector2 getParameter(const Vector2 &point)
  {
    // This can potentially have a divide by zero if the box
    // has a size dimension of 0.
    return Vector2(
       ( point.x() - _min.x() ) / ( _max.x() - _min.x() ),
       ( point.y() - _min.y() ) / ( _max.y() - _min.y() ));
  }

  bool intersectsBox(const Box2 &box) 
  {
    // using 4 splitting planes to rule out intersections
    return !(box._max.x() < _min.x() || box._min.x() > _max.x() || box._max.y() < _min.y() || box._min.y() > _max.y());
  }

  Vector2 clamp(const Vector2 &point)
  {
    return Vector2(point).clamp( _min, _max );
  }

  float distance(const Vector2 &point)
  {
    Vector2 clampedPoint(point);
    clampedPoint.clamp( _min, _max );
    return (clampedPoint - point).length();
  }

  Box2 &intersect(const Box2 &box)
  {
    _min = max(_min, box._min);
    _max = min(_max, box._max );

    return *this;
  }

  Box2 &setUnion(const Box2 &box)
  {
     _min = min(_min, box._min);
     _max = max(_max, box._max);

     return *this;
  }

  Box2 &translate(const Vector2 &offset)
  {
    _min += offset;
    _max += offset;

    return *this;
  }

  bool operator == (const Box2 &box)
  {
    return box._min == _min && box._max == _max;
  }
};

}
}


#endif //THREE_QT_BOX2_H
