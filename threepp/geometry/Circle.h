//
// Created by byter on 12/21/17.
//

#ifndef THREEPP_CIRCLE_H
#define THREEPP_CIRCLE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

class CircleParams
{
protected:
  float _radius = 50;
  unsigned _segments = 8;
  float _thetaStart = 0;
  float _thetaLength = (float)M_PI * 2;

  CircleParams(float radius, unsigned segments, float thetaStart, float thetaLength)
  : _radius(radius), _segments(segments), _thetaStart(thetaStart), _thetaLength(thetaLength) {}
};

class DLX Circle : public LinearGeometry, private CircleParams
{
  Circle(const Circle &circle) : LinearGeometry(circle), CircleParams(circle) {}

protected:
  Circle(float radius, unsigned segments, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Circle>;
  static Ptr make(float radius, unsigned segments, float thetaStart, float thetaLength) {
    return Ptr(new Circle(radius, segments, thetaStart, thetaLength));
  }

  Circle *cloned() const override {
    return new Circle(*this);
  }
};

namespace buffer {

class DLX Circle : public BufferGeometry, private CircleParams
{
  friend class three::geometry::Circle;

  Circle(const Circle &circle);

protected:
  Circle(float radius, unsigned segments, float thetaStart, float thetaLength);

public:
  using Ptr = std::shared_ptr<Circle>;
  static Ptr make(float radius, unsigned segments, float thetaStart=0, float thetaLength=2.0f*(float)M_PI) {
    return Ptr(new Circle(radius, segments, thetaStart, thetaLength));
  }

  Circle *cloned() const override {
    return new Circle(*this);
  }
};

}

}
}


#endif //THREEPP_CIRCLE_H
