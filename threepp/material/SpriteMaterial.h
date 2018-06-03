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
class DLX SpriteMaterial : public MaterialT<material::Colored>
{
  SpriteMaterial()
     : MaterialT(material::InfoT<SpriteMaterial>(), material::Typer(this))
  {
    this->morphTargets = false;
    this->morphNormals = false;
    this->skinning = false;
  }

  SpriteMaterial(const SpriteMaterial &material)
     : MaterialT(material, material::InfoT<SpriteMaterial>(), material::Typer(this))
  {}

public:
  float rotation = 0;

  math::Vector2 uvOffset;
  math::Vector2 uvScale;

  using Ptr = std::shared_ptr<SpriteMaterial>;
  static Ptr make() {return Ptr(new SpriteMaterial());}

  bool fog = false;
  bool lights = false;

  SpriteMaterial *cloned() const override {
    return new SpriteMaterial(*this);
  }

  bool transparent() const override {return opacity < 1.0f;}
};

}
#endif //THREEPP_SPRITEMATERIAL_H
