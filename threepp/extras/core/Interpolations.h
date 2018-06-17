/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 *
 * Bezier Curves formulas obtained from
 * http://en.wikipedia.org/wiki/Bézier_curve
 */

#include <threepp/math/Vector2.h>

namespace three {
namespace extras {
namespace interpolations {

float CatmullRom(float t, float p0, float p1, float p2, float p3);

float QuadraticBezier(float t, float p0, float p1, float p2);

float CubicBezier(float t, float p0, float p1, float p2, float p3);

}
}
}

//export { CatmullRom, QuadraticBezier, CubicBezier };
