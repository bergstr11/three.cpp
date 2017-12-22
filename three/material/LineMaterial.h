//
// Created by byter on 26.10.17.
//

#ifndef THREE_QT_LINEBASICMATERIAL_H
#define THREE_QT_LINEBASICMATERIAL_H

#include "Material.h"
#include <core/Color.h>

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

public:
  unsigned linewidth = 1;
  LineCap linecap = LineCap::round;
  LineJoin linejoin = LineJoin::round;

  bool lights = false;

  using Ptr = std::shared_ptr<LineBasicMaterial>;
  static Ptr make(Colors colors=Colors::Vertex) {
    Ptr p(new LineBasicMaterial());
    p->vertexColors = colors;
    return p;
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

public:
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
