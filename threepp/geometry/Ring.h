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

  geometry::Typer mktyper() {return geometry::Typer(this);}
};

class DLX Ring : public LinearGeometry, public RingParams
{
  Ring(const Ring &ring) : LinearGeometry(mktyper()), RingParams(ring) {}

protected:
  Ring(const RingParams &params);

public:
  using Ptr = std::shared_ptr<Ring>;

  static Ptr make(const RingParams &params=RingParams()) {
    return Ptr(new Ring(params));
  }

  Ring *cloned() const override {
    return new Ring(*this);
  }
};

namespace buffer {

class Ring : public BufferGeometry, public RingParams
{
  friend class geometry::Ring;

  Ring(const Ring &ring) : BufferGeometry(ring, mktyper()), RingParams(ring) {}

protected:
  Ring(const RingParams &params);

public:
  using Ptr = std::shared_ptr<Ring>;

  static Ptr make(const RingParams &params=RingParams()) {
    return Ptr(new Ring(params));
  }

  Ring *cloned() const override {
    return new Ring(*this);
  }
};

}

}
}

#endif //THREE_PP_RING_H
