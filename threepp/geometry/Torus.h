//
// Created by byter on 2/9/18.
//

#ifndef THREE_PP_TORUS_H
#define THREE_PP_TORUS_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {
namespace geometry {

struct TorusParams
{
  float radius = 1.0f;
  float tube = 0.4f;
  unsigned radialSegments = 8;
  unsigned tubularSegments = 6;
  float arc = (float)M_PI * 2;
};

class Torus : public LinearGeometry, private TorusParams
{
protected:
  Torus(const TorusParams &params);

public:
  using Ptr = std::shared_ptr<Torus>;

  static Ptr make(const TorusParams &params=TorusParams()) {
    return Ptr(new Torus(params));
  }

  Torus *cloned() const override {
    return new Torus(*this);
  }
};

namespace buffer {

class Torus : public BufferGeometry, private TorusParams
{
  friend class geometry::Torus;

protected:
  Torus(const TorusParams &params);

public:
  using Ptr = std::shared_ptr<Torus>;

  static Ptr make(const TorusParams &params=TorusParams()) {
    return Ptr(new Torus(params));
  }

  Torus *cloned() const override {
    return new Torus(*this);
  }
};

}

}
}

#endif //THREE_PP_TORUS_H
