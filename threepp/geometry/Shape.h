//
// Created by byter on 8/11/18.
//

#ifndef THREE_PP_SHAPE_H
#define THREE_PP_SHAPE_H

#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/extras/core/ShapePath.h>

namespace three {
namespace geometry {

class DLX Shape : public LinearGeometry
{
  unsigned _curveSegments;

  Shape(const Shape &shape) : LinearGeometry(shape, geometry::Typer(this)), _curveSegments(shape._curveSegments) {}

protected:
  Shape(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments);

public:
using Ptr = std::shared_ptr<Shape>;
static Ptr make(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments=12)
{
  return Ptr(new Shape(shapes, curveSegments));
}

Shape *cloned() const override {
  return new Shape(*this);
}
};

namespace buffer {

class DLX Shape : public BufferGeometry
{
  friend class three::geometry::Shape;

  Shape(const Shape &shape) : BufferGeometry(shape, geometry::Typer(this)) {}

protected:
  Shape(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments);

public:
  using Ptr = std::shared_ptr<Shape>;
  static Ptr make(const std::vector<extras::Shape::Ptr> &shapes, unsigned curveSegments=12)
  {
    return Ptr(new Shape(shapes, curveSegments));
  }

  Shape *cloned() const override {
    return new Shape(*this);
  }
};

}

}
}


#endif //THREE_PP_SHAPE_H
