//
// Created by byter on 11/17/17.
//

#ifndef THREEPP_POINTSMATERIAL_H
#define THREEPP_POINTSMATERIAL_H

#include "Material.h"

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  opacity: <float>,
 *  map: new THREE.Texture( <Image> ),
 *
 *  size: <float>,
 *  sizeAttenuation: <bool>
 * }
 */

struct PointsMaterial : public MaterialT<material::Colored>
{
  float size = 1;
  bool sizeAttenuation = true;

protected:
  PointsMaterial() : MaterialT(material::ResolverT<PointsMaterial>::make(*this))
  {
    lights = false;
  }

public:
  using Ptr = std::shared_ptr<PointsMaterial>;
  static Ptr make(Color color, float opacity, Texture::Ptr map) {
    auto p = Ptr(new PointsMaterial());
    p->color = color;
    p->opacity = opacity;
    p->map = map;
    return p;
  }

  static Ptr make(Color color, float opacity=1.0f, float size=1.0f, bool sizeAttenuation=true) {
    auto p = Ptr(new PointsMaterial());
    p->color = color;
    p->opacity = opacity;
    p->size = size;
    p->sizeAttenuation = sizeAttenuation;
    return p;
  }
};

}

#endif //THREEPP_POINTSMATERIAL_H
