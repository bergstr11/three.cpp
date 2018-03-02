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
  float size;
  bool sizeAttenuation;

protected:
  PointsMaterial() : MaterialT(material::ResolverT<PointsMaterial>::make(*this))
  {
    lights = false;
  }

public:
  using Ptr = std::shared_ptr<PointsMaterial>;
  static Ptr make(Color color) {
    auto p = Ptr(new PointsMaterial());
    p->color = color;
    return p;
  }
};

}

#endif //THREEPP_POINTSMATERIAL_H
