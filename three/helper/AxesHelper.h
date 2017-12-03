//
// Created by byter on 20.08.17.
//

#ifndef THREE_QT_AXISHELPER_H
#define THREE_QT_AXISHELPER_H

#include <objects/Line.h>
#include <memory>
#include <core/BufferAttribute.h>

namespace three {
namespace helper {

class AxesHelper : public LineSegments {

  static BufferGeometry::Ptr createGeometry(float size)
  {
    std::vector<float> vertices {
       0, 0, 0, size, 0, 0,
       0, 0, 0, 0, size, 0,
       0, 0, 0, 0, 0, size
    };

    std::vector<float> colors {
       1, 0, 0, 1, 0.6, 0,
       0, 1, 0, 0.6, 1, 0,
       0, 0, 1, 0, 0.6, 1
    };

    BufferGeometry::Ptr geometry = BufferGeometry::make();
    geometry->setPosition(BufferAttributeT<float>::make(vertices, 3, true));
    geometry->setColor(BufferAttributeT<float>::make(colors, 3, true));
    return geometry;
  }

  AxesHelper(size_t size)
     : Object3D(object::ResolverT<LineSegments>::make(*this)),
       LineSegments(createGeometry(size), LineBasicMaterial::make(Colors::Vertex)) {}

public:
  using Ptr = std::shared_ptr<AxesHelper>;
  static Ptr make(std::string name, size_t size=1) {
    Ptr p(new AxesHelper(size));
    p->_name = name;
    return p;
  }
};

}
}
#endif //THREE_QT_AXISHELPER_H
