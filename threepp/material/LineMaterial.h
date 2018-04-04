//
// Created by byter on 26.10.17.
//

#ifndef THREEPP_LINEBASICMATERIAL_H
#define THREEPP_LINEBASICMATERIAL_H

#include "Material.h"
#include <threepp/core/Color.h>

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  opacity: <float>,
 *
 *  linewidth: <float>,
 *  linecap: "round",
 *  linejoin: "round"
 * }
 */
class LineBasicMaterial : public MaterialT<material::Colored>
{
protected:
  LineBasicMaterial() : MaterialT(material::ResolverT<LineBasicMaterial>::make(*this)) {}
  LineBasicMaterial(material::Resolver::Ptr resolver) : MaterialT(resolver) {}

  LineBasicMaterial(const LineBasicMaterial &material)
     : MaterialT(material, material::ResolverT<LineBasicMaterial>::make(*this)) {}
  LineBasicMaterial(const LineBasicMaterial &material, material::Resolver::Ptr resolver)
     : MaterialT(material, resolver) {}

public:
  unsigned linewidth = 1;
  LineCap linecap = LineCap::round;
  LineJoin linejoin = LineJoin::round;

  bool lights = false;

  using Ptr = std::shared_ptr<LineBasicMaterial>;
  static Ptr make(Colors colors=Colors::None) {
    Ptr p(new LineBasicMaterial());
    p->vertexColors = colors;
    return p;
  }

  LineBasicMaterial *cloned() const override {
    return new LineBasicMaterial(*this);
  }
};

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  opacity: <float>,
 *
 *  linewidth: <float>,
 *
 *  scale: <float>,
 *  dashSize: <float>,
 *  gapSize: <float>
 * }
 */
class LineDashedMaterial : public LineBasicMaterial
{
protected:
  LineDashedMaterial() : LineBasicMaterial(material::ResolverT<LineDashedMaterial>::make(*this)) {}
  LineDashedMaterial(const LineDashedMaterial &material)
     : LineBasicMaterial(material, material::ResolverT<LineDashedMaterial>::make(*this)) {}

public:
  float scale = 1;
  unsigned dashSize = 3;
  unsigned gapSize = 1;

  using Ptr = std::shared_ptr<LineDashedMaterial>;
  static Ptr make() {
    return Ptr(new LineDashedMaterial());
  }

  LineDashedMaterial *cloned() const override {
    return new LineDashedMaterial(*this);
  }
};

}
#endif //THREEPP_LINEBASICMATERIAL_H
