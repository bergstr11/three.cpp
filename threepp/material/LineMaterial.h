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
  LineBasicMaterial() : MaterialT(material::InfoT<LineBasicMaterial>(), material::Typer(this)) {}

  LineBasicMaterial(material::Info info, const material::Typer &typer) : MaterialT(info, typer)
  {
    this->typer.allow<LineBasicMaterial>();
  }

  LineBasicMaterial(const LineBasicMaterial &material)
     : MaterialT(material, material::InfoT<LineBasicMaterial>(), material::Typer(this)) {}

  LineBasicMaterial(const LineBasicMaterial &material, const material::Info &info, const material::Typer &typer)
     : MaterialT(material, info, typer)
  {
    this->typer.allow<LineBasicMaterial>();
  }

public:
  float linewidth = 1;
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

  bool transparent() const override {return opacity < 1.0f;}
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
  LineDashedMaterial()
     : LineBasicMaterial(material::InfoT<LineDashedMaterial>(),
                         material::Typer(this).allow<LineBasicMaterial>())
  {}
  LineDashedMaterial(const LineDashedMaterial &material)
     : LineBasicMaterial(material, material::InfoT<LineDashedMaterial>(),
                         material::Typer(this).allow<LineBasicMaterial>())
  {}

public:
  float scale = 1;
  float dashSize = 3;
  float gapSize = 1;

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
