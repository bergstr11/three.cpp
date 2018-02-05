//
// Created by byter on 03.09.17.
//

#ifndef THREEPP_SPRITEMATERIAL_H
#define THREEPP_SPRITEMATERIAL_H

#include "Material.h"

namespace three {

/**
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  opacity: <float>,
 *  map: new THREE.Texture( <Image> ),
 *
 *	uvOffset: new THREE.Vector2(),
 *	uvScale: new THREE.Vector2()
 * }
 */
class SpriteMaterial : public MaterialT<material::Colored>
{
  SpriteMaterial() : MaterialT(material::ResolverT<SpriteMaterial>::make(*this))
  {
    this->morphTargets = false;
    this->morphNormals = false;
    this->skinning = false;
  }

public:
  float rotation = 0;

  math::Vector2 uvOffset;
  math::Vector2 uvScale;

  using Ptr = std::shared_ptr<SpriteMaterial>;
  static Ptr make() {return Ptr(new SpriteMaterial());}

  bool fog = false;
  bool lights = false;
  
};
}
#endif //THREEPP_SPRITEMATERIAL_H
