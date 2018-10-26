//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_EXTRUDE_H
#define THREE_PP_EXTRUDE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/extras/ShapeUtils.h>
#include <threepp/extras/core/Shape.h>
#include <threepp/extras/core/CurvePath.h>

namespace three {
namespace geometry {

struct UVGenerator
{
  using Ptr = std::shared_ptr<UVGenerator>;

  virtual void generateTopUV(const attribute::growing_t<float, Vertex> &positions,
                             attribute::growing_t<float, UV> &uvs,
                             unsigned indexA,
                             unsigned indexB,
                             unsigned indexC) = 0;

  virtual void generateSideWallUV(const attribute::growing_t<float, Vertex> &positions,
                                  attribute::growing_t<float, UV> &uvs,
                                  unsigned indexA,
                                  unsigned indexB,
                                  unsigned indexC,
                                  unsigned indexD ) = 0;
};

struct WorldUVGenerator : public UVGenerator
{
  static Ptr make() {
    return Ptr(new WorldUVGenerator());
  }

  void generateTopUV(const attribute::growing_t<float, Vertex> &positions,
                     attribute::growing_t<float, UV> &uvs,
                     unsigned indexA,
                     unsigned indexB,
                     unsigned indexC) override;

  void generateSideWallUV(const attribute::growing_t<float, Vertex> &positions,
                          attribute::growing_t<float, UV> &uvs,
                          unsigned indexA,
                          unsigned indexB,
                          unsigned indexC,
                          unsigned indexD ) override;
};

namespace buffer {
struct Builder;
class Extrude;
}
struct ShapeGroup
{
  friend struct buffer::Builder;
  friend class buffer::Extrude;

  ShapeGroup(unsigned group, unsigned shapeStart, unsigned shapeCount)
     : group(group), shapeStart(shapeStart), shapeCount(shapeCount) {}

private:
  int group;
  unsigned shapeStart, shapeCount;
  int posStart=-1, posCount=0;
};

struct ExtrudeOptions
{
  unsigned curveSegments = 12;
  unsigned steps = 1;
  float depth = 100;
  bool bevelEnabled = true;
  float bevelThickness = 6;
  float bevelSize = bevelThickness - 2;
  unsigned bevelSegments = 3;
  std::vector<ShapeGroup> shapeGroups;

  extras::CurvePath::Ptr extrudePath = nullptr;
  UVGenerator::Ptr uvGenerator = nullptr;
};

/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 *
 * Creates extruded geometry from a path shape.
 *
 * parameters = {
 *
 *  curveSegments: <int>, // number of points on the curves
 *  steps: <int>, // number of points for z-side extrusions / used for subdividing segments of extrude spline too
 *  amount: <float>, // Depth to extrude the shape
 *
 *  bevelEnabled: <bool>, // turn on bevel
 *  bevelThickness: <float>, // how deep into the original shape bevel goes
 *  bevelSize: <float>, // how far from shape outline is bevel
 *  bevelSegments: <int>, // number of bevel layers
 *
 *  extrudePath: <THREE.Curve> // curve to extrude shape along
 *
 *  UVGenerator: <Object> // object that provides UV generator functions
 *
 * }
 */
class DLX Extrude : public LinearGeometry
{
protected:
  std::vector<extras::Shape::Ptr> _shapes;
  const ExtrudeOptions _options;

  Extrude(const Extrude &extrude) : LinearGeometry(extrude) {}

  Extrude(const std::vector<extras::Shape::Ptr> &shapes, const ExtrudeOptions &options);

  Extrude(extras::Shape::Ptr shape, const ExtrudeOptions &options);

public:
  using Ptr = std::shared_ptr<Extrude>;

  static Ptr make(const std::vector<extras::Shape::Ptr> &shapes, const ExtrudeOptions &options) {
    return Ptr(new Extrude(shapes, options));
  }

  static Ptr make(extras::Shape::Ptr shape, const ExtrudeOptions &options) {
    return Ptr(new Extrude(shape, options));
  }

  Extrude *cloned() const override {
    return new Extrude(*this);
  }
};

namespace buffer {

class DLX Extrude : public BufferGeometry
{
  friend class three::geometry::Extrude;

protected:
  Extrude(const Extrude &extrude) : BufferGeometry(extrude) {}
  Extrude(const std::vector<extras::Shape::Ptr> &shapes, const ExtrudeOptions &options);

public:
  using Ptr = std::shared_ptr<Extrude>;

  static Ptr make(const std::vector<extras::Shape::Ptr> &shapes, const ExtrudeOptions &options) {
    return Ptr(new Extrude(shapes, options));
  }

  Extrude *cloned() const override {
    return new Extrude(*this);
  }
};

}

}
}


#endif //THREE_PP_EXTRUDE_H
