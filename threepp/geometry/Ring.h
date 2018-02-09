//
// Created by byter on 2/8/18.
//

#ifndef THREE_PP_RING_H
#define THREE_PP_RING_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct RingParams
{
  float innerRadius = 0.5f;
  float outerRadius = 1.0f;
  unsigned thetaSegments = 8;
  unsigned phiSegments = 1;
  float thetaStart = 0;
  float thetaLength = (float)M_PI * 2;

  void validate() {
    thetaSegments = std::max(3u, thetaSegments);
    phiSegments = std::max(1u, phiSegments);
  }
};

class Ring : public LinearGeometry, private RingParams
{
protected:
  Ring(const RingParams &params);

public:
  using Ptr = std::shared_ptr<Ring>;

  static Ptr make(const RingParams &params=RingParams()) {
    return Ptr(new Ring(params));
  }
};

namespace buffer {

class Ring : public BufferGeometry, private RingParams
{
  friend class geometry::Ring;

protected:
  Ring(const RingParams &params);

public:
  using Ptr = std::shared_ptr<Ring>;

  static Ptr make(const RingParams &params=RingParams()) {
    return Ptr(new Ring(params));
  }
};

}

}
}

#endif //THREE_PP_RING_H
