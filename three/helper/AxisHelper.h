//
// Created by byter on 20.08.17.
//

#ifndef THREE_QT_AXISHELPER_H
#define THREE_QT_AXISHELPER_H

#include <objects/Line.h>
#include <memory>

namespace three {
namespace helper {

class AxisHelper : public Line {

  static BufferGeometry::Ptr createGeometry(float size)
  {
    BufferGeometry::Ptr geometry = BufferGeometry::make();
    //geometry.addAttribute( 'position', new Float32BufferAttribute( vertices, 3 ) );
    //geometry.addAttribute( 'color', new Float32BufferAttribute( colors, 3 ) );
    return geometry;
  }

  AxisHelper(size_t size) : Line(createGeometry(size), LineBasicMaterial::make(size)) {}

public:
  using Ptr = std::shared_ptr<AxisHelper>;
  static Ptr make(size_t size) {return std::shared_ptr<AxisHelper>(new AxisHelper(size));}
};

}
}
#endif //THREE_QT_AXISHELPER_H
