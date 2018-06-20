//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_SHAPEUTILS_H
#define THREE_PP_SHAPEUTILS_H

#include <vector>
#include <threepp/math/Vector2.h>
#include "Earcut.h"

namespace mapbox {
namespace util {

template <>
struct nth<0, three::math::Vector2> {
  inline static float get(const three::math::Vector2 &t) {
    return t.x();
  };
};
template <>
struct nth<1, three::math::Vector2> {
  inline static float get(const three::math::Vector2 &t) {
    return t.y();
  };
};

} // util
} // mapbox

namespace three {
namespace extras {

namespace shapeutils
{

using Face = std::array<uint32_t, 3>;

// calculate area of the contour polygon

inline float area(const std::vector<math::Vector2> &contour)
{
   auto n = contour.size();
   float a = 0.0f;

   for (unsigned p = n - 1, q = 0u; q < n; p = q++ ) {

     a += contour[ p ].x() * contour[ q ].y() - contour[ q ].x() * contour[ p ].y();
   }

   return a * 0.5f;
}

inline bool isClockWise(const std::vector<math::Vector2> &pts)
{
   return area( pts ) < 0;
}

inline void removeDupEndPts( std::vector<math::Vector2> &points ) {

  auto l = points.size();

  if ( l > 2 && points[ l - 1 ] == points[ 0 ]) {

    points.pop_back();
  }
}

inline std::vector<Face> triangulateShape(std::vector<math::Vector2> &contour,
                                          std::vector<std::vector<math::Vector2>> &holes)
{
  std::vector<Face> faces;            // final array of vertex indices like [ [ a,b,d ], [ b,c,d ] ]
  std::vector<std::vector<math::Vector2>> polygon;

  removeDupEndPts( contour );
  polygon.push_back(contour);

  for(auto &hole : holes) {
    removeDupEndPts(hole);
    polygon.push_back(hole);
  }

  std::vector<uint32_t> triangles = mapbox::earcut<uint32_t>(polygon);

  for ( unsigned i = 0; i < triangles.size(); i += 3 ) {

    faces.push_back({triangles[i], triangles[i+1], triangles[i+2]});
  }

  return faces;
}


}
}
}
#endif //THREE_PP_SHAPEUTILS_H
