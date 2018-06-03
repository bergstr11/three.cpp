//
// Created by byter on 12/21/17.
//

#ifndef THREEPP_CIRCLE_H
#define THREEPP_CIRCLE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct CircleParams
{
  float radius = 50;
  unsigned segments = 8;
  float thetaStart = 0;
  float thetaLength = (float)M_PI * 2;
};

class DLX Circle : public LinearGeometry
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

  Circle *cloned() const override {
    return LinearGeometry::setTyper(new Circle(*this));
  }
};

namespace buffer {

class DLX Circle : public BufferGeometry
{
  friend class three::geometry::Circle;

protected:
  Circle(float radius, unsigned segments, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Circle>;
  static Ptr make(float radius, unsigned segments, float thetaStart=0, float thetaLength=(float)2*M_PI) {
    return Ptr(new Circle(radius, segments, thetaStart, thetaLength));
  }

  static Ptr make(const CircleParams &params=CircleParams()) {
    return Ptr(new Circle(params.radius, params.segments, params.thetaStart, params.thetaLength));
  }

  Circle *cloned() const override {
    return BufferGeometry::setTyper(new Circle(*this));
  }
};

}

}
}


#endif //THREEPP_CIRCLE_H
