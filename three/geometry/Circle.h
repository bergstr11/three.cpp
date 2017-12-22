//
// Created by byter on 12/21/17.
//

#ifndef THREEPP_CIRCLE_H
#define THREEPP_CIRCLE_H

#include <core/StaticGeometry.h>
#include <core/BufferGeometry.h>

namespace three {
namespace geometry {

struct CircleParams
{
  float radius = 50;
  unsigned segments = 8;
  float thetaStart = 0;
  float thetaLength = (float)M_PI * 2;
};

class Circle : public StaticGeometry
{
  float _radius;
  unsigned _segments;
  float _thetaStart, _thetaLength;

protected:
  Circle(float radius, unsigned segments, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Circle>;
  static Ptr make(float radius, unsigned segments, float thetaStart, float thetaLength) {
    return Ptr(new Circle(radius, segments, thetaStart, thetaLength));
  }

  static Ptr make(const CircleParams &params=CircleParams()) {
    return Ptr(new Circle(params.radius, params.segments, params.thetaStart, params.thetaLength));
  }
};

namespace buffer {

class Circle : public BufferGeometry
{
  friend class three::geometry::Circle;

protected:
  Circle(float radius, unsigned segments, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Circle>;
  static Ptr make(float radius, unsigned segments, float thetaStart, float thetaLength) {
    return Ptr(new Circle(radius, segments, thetaStart, thetaLength));
  }

  static Ptr make(const CircleParams &params=CircleParams()) {
    return Ptr(new Circle(params.radius, params.segments, params.thetaStart, params.thetaLength));
  }
};

}

}
}


#endif //THREEPP_CIRCLE_H
