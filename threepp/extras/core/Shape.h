#pragma once

#include <threepp/util/sole.h>
#include "Path.h"

namespace three {
namespace extras {

class ShapePath;

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 * Defines a 2d shape plane using paths.
 **/

// STEP 1 Create a path.
// STEP 2 Turn path into shape.
// STEP 3 ExtrudeGeometry takes in Shape/Shapes
// STEP 3a - Extract points from each shape, turn to vertices
// STEP 3b - Triangulate each shape, add faces.

class Shape : public Path
{
  friend ShapePath;

protected:
  std::vector<Path::Ptr> _holes;

  Shape(const std::vector<math::Vector2> &points) : Path(points), uuid(sole::uuid0())
  {}

  Shape(const std::vector<Curve::Ptr> &curves) : Path(curves), uuid(sole::uuid0())
  {}

public:
  const sole::uuid uuid;

  using Ptr = std::shared_ptr<Shape>;

  static Ptr make(const std::vector<math::Vector2> &points = std::vector<math::Vector2>())
  {
    return Ptr(new Shape(points));
  }

  static Ptr make(const std::vector<Curve::Ptr> &curves)
  {
    return Ptr(new Shape(curves));
  }

  std::vector<std::vector<math::Vector2>> getPointsHoles(unsigned divisions,
                                                         std::vector<std::vector<math::Vector2>> &holesPts)
  {
    for (const auto &hole : _holes) {

      std::vector<math::Vector2> pts;
      hole->getPoints(divisions, pts);

      holesPts.push_back(pts);
    }

    return holesPts;
  }

  // get points of shape and holes (keypoints based on segments parameter)
  void extractPoints(unsigned divisions,
                     std::vector<math::Vector2> &shape,
                     std::vector<std::vector<math::Vector2>> &holes)
  {
    getPoints(divisions, shape);
    getPointsHoles(divisions, holes);
  }
};

}
}
