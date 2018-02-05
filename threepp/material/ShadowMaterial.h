//
// Created by byter on 12/18/17.
//

#ifndef THREEPP_SHADOWMATERIAL_H
#define THREEPP_SHADOWMATERIAL_H

#include "Material.h"
#include <threepp/core/Color.h>

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 *
 * parameters = {
 *  color: <THREE.Color>,
 *  opacity: <float>
 * }
 */
class ShadowMaterial : public MaterialT<material::Colored>
{
private:
  ShadowMaterial() : MaterialT(material::ResolverT<ShadowMaterial>::make(*this))
  {
    this->color = 0x000000;
    this->opacity = 1.0f;
    this->lights = true;
    this->transparent = true;
  }

public:
  using Ptr = std::shared_ptr<ShadowMaterial>;

  static Ptr make() {
    return Ptr(new ShadowMaterial());
  }
};

}

#endif //THREEPP_SHADOWMATERIAL_H
