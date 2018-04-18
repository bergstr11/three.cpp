//
// Created by byter on 20.08.17.
//

#ifndef THREEPP_AXISHELPER_H
#define THREEPP_AXISHELPER_H

#include <memory>

#include <threepp/objects/Line.h>
#include <threepp/core/BufferAttribute.h>

namespace three {
namespace helper {

class Axes : public LineSegments
{
  static BufferGeometry::Ptr createGeometry(float size)
  {
    auto vertices = attribute::prealloc<float, Vertex>(
       {{0.0f, 0.0f, 0.0f}, {size, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, size, 0.0f},
        {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, size}});

    auto colors = attribute::prealloc<float, Color>(
       {{1.0f, 0.0f, 0.0f}, {1.0f, 0.6f, 0.0f},
        {0.0f, 1.0f, 0.0f}, {0.6f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.6f, 1.0f}});

    BufferGeometry::Ptr geometry = BufferGeometry::make();
    geometry->setPosition(vertices);
    geometry->setColor(colors);
    return geometry;
  }

  Axes(size_t size)
     : Object3D(), LineSegments(createGeometry(size), LineBasicMaterial::make(Colors::Vertex)) {}

public:
  using Ptr = std::shared_ptr<Axes>;
  static Ptr make(std::string name, size_t size=1) {
    Ptr p(new Axes(size));
    p->_name = name;
    return p;
  }
};

}
}
#endif //THREEPP_AXISHELPER_H
