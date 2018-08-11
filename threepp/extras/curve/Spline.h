//
// Created by byter on 6/15/18.
//

#ifndef THREE_PP_SPLINE_H
#define THREE_PP_SPLINE_H

#include <threepp/extras/core/Curve.h>
#include <threepp/extras/core/Interpolations.h>
#include <threepp/math/Vector2.h>

namespace three {
namespace extras {
namespace curve {

class Spline : public Curve
{
protected:
  std::vector<math::Vector2> points;

  explicit Spline(const std::vector<math::Vector2> &points)
     : points(points) {}

public:
  using Ptr = std::shared_ptr<Spline>;

  static Ptr make(const std::vector<math::Vector2> &points)
  {
    return Ptr(new Spline(points));
  }

  math::Vector2 getPoint( float t ) override
  {
    float p = ( points.size() - 1 ) * t;

    int intPoint = (int)std::floor( p );
    float weight = p - intPoint;

    const math::Vector2 &p0 = points[ intPoint == 0 ? intPoint : intPoint - 1 ];
    const math::Vector2 &p1 = points[ intPoint ];
    const math::Vector2 &p2 = points[ intPoint > points.size() - 2 ? points.size() - 1 : intPoint + 1 ];
    const math::Vector2 &p3 = points[ intPoint > points.size() - 3 ? points.size() - 1 : intPoint + 2 ];

    return math::Vector2(
       interpolations::CatmullRom( weight, p0.x(), p1.x(), p2.x(), p3.x() ),
       interpolations::CatmullRom( weight, p0.y(), p1.y(), p2.y(), p3.y() )
    );
  }

  math::Vector2 start() const override {
    return points[ 0 ];
  }
};

}
}
}
#endif //THREE_PP_SPLINE_H
