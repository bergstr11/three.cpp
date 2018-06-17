//
// Created by byter on 6/15/18.
//

#ifndef THREE_PP_QUADRATICBEZIER_H
#define THREE_PP_QUADRATICBEZIER_H

#include <threepp/extras/core/Curve.h>
#include <threepp/extras/core/Interpolations.h>
#include <threepp/math/Vector2.h>

namespace three {
namespace extras {
namespace curve {

class QuadraticBezier : public Curve
{
protected:
  math::Vector2 v0, v1, v2;

  QuadraticBezier(const math::Vector2 &v0, const math::Vector2 &v1, const math::Vector2 &v2)
     : v0(v0), v1(v1), v2(v2) {}

public:
  using Ptr = std::shared_ptr<QuadraticBezier>;

  static Ptr make(const math::Vector2 &v0, const math::Vector2 &v1, const math::Vector2 &v2)
  {
    return Ptr(new QuadraticBezier(v0, v1, v2));
  }

  math::Vector2 getPoint( float t) override
  {
    return math::Vector2(
       interpolations::QuadraticBezier( t, v0.x(), v1.x(), v2.x() ),
       interpolations::QuadraticBezier( t, v0.y(), v1.y(), v2.y() )
    );
  }
};

}
}
}

#endif //THREE_PP_QUADRATICBEZIER_H
