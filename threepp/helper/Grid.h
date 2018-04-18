//
// Created by byter on 1/28/18.
//

#ifndef THREEPP_GRIDHELPER_H
#define THREEPP_GRIDHELPER_H

#include <threepp/objects/Line.h>
#include <threepp/core/Color.h>

namespace three {
namespace helper {

class Grid : public LineSegments
{
public:
  struct Options
  {
    unsigned size = 10;
    unsigned divisions = 10;
    Color color1 {0x444444};
    Color color2 {0x888888};
  };

protected:
  static BufferGeometry::Ptr createGeometry(const Options &o)
  {
    float center = (float) o.divisions / 2;
    float step = (float) o.size / o.divisions;
    float halfSize = (float) o.size / 2;

    auto vertices = attribute::prealloc<float, Vertex>((o.divisions + 1) * 4);
    auto colors = attribute::prealloc<float, Color>((o.divisions + 1) * 4);

    float k = -halfSize;
    for (unsigned  i = 0; i <= o.divisions; i++) {

      vertices->next() = {-halfSize, 0, k};
      vertices->next() = {halfSize, 0, k};
      vertices->next() = {k, 0, -halfSize};
      vertices->next() = {k, 0, halfSize};

      const Color &color = i == center ? o.color1 : o.color2;

      for (auto j = 0; j < 4; j++) colors->next() = color;
      k += step;
    }

    BufferGeometry::Ptr geometry = BufferGeometry::make();
    geometry->setPosition(vertices);
    geometry->setColor(colors);

    return geometry;
  }

  Grid(const Options &options)
     : Object3D(), LineSegments(createGeometry(options), LineBasicMaterial::make(Colors::Vertex))
  {}

public:
  using Ptr = std::shared_ptr<Grid>;
  static Ptr make(const Options &options) {
    return Ptr(new Grid(options));
  }
};

}
}

#endif //THREEPP_GRIDHELPER_H
