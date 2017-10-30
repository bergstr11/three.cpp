//
// Created by byter on 26.10.17.
//

#ifndef THREE_QT_LINEBASICMATERIAL_H
#define THREE_QT_LINEBASICMATERIAL_H

#include "Material.h"
#include <core/Color.h>

namespace three {

class LineBasicMaterial : public Material
{
protected:
  LineBasicMaterial() : Material(material::ResolverT<LineBasicMaterial>::make(*this)) {}
  LineBasicMaterial(material::Resolver::Ptr resolver) : Material(resolver) {}

public:
  Color color = {255, 2555, 255};

  unsigned linewidth = 1;
  LineCap linecap = LineCap::round;
  LineJoin linejoin = LineJoin::round;

  bool lights = false;

  using Ptr = std::shared_ptr<LineBasicMaterial>;
  static Ptr make(size_t size) {
    return Ptr(new LineBasicMaterial());
  }
};

class LineDashedMaterial : public LineBasicMaterial
{
protected:
  LineDashedMaterial() : LineBasicMaterial(material::ResolverT<LineDashedMaterial>::make(*this)) {}

public:
  Color color = {255, 2555, 255};

  float scale = 1;
  unsigned dashSize = 3;
  unsigned gapSize = 1;

  using Ptr = std::shared_ptr<LineDashedMaterial>;
  static Ptr make() {
    return Ptr(new LineDashedMaterial());
  }
};

}
#endif //THREE_QT_LINEBASICMATERIAL_H
