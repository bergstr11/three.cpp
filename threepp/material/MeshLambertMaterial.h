//
// Created by byter on 11/4/17.
//

#ifndef THREEPP_MESHLAMBERTMATERIAL_H
#define THREEPP_MESHLAMBERTMATERIAL_H

#include <threepp/core/Color.h>
#include "Material.h"

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  opacity: <float>,
 *
 *  map: new THREE.Texture( <Image> ),
 *
 *  lightMap: new THREE.Texture( <Image> ),
 *  lightMapIntensity: <float>
 *
 *  aoMap: new THREE.Texture( <Image> ),
 *  aoMapIntensity: <float>
 *
 *  emissive: <hex>,
 *  emissiveIntensity: <float>
 *  emissiveMap: new THREE.Texture( <Image> ),
 *
 *  specularMap: new THREE.Texture( <Image> ),
 *
 *  alphaMap: new THREE.Texture( <Image> ),
 *
 *  envMap: new THREE.TextureCube( [posx, negx, posy, negy, posz, negz] ),
 *  combine: THREE.Multiply,
 *  reflectivity: <float>,
 *  refractionRatio: <float>,
 *
 *  wireframe: <boolean>,
 *  wireframeLinewidth: <float>,
 *
 *  skinning: <bool>,
 *  morphTargets: <bool>,
 *  morphNormals: <bool>
 * }
 */

struct MeshLambertMaterial : public MaterialT<
   material::Colored,
   material::AoMap,
   material::LightMap,
   material::Emissive,
   material::EnvMap,
   material::SpecularMap,
   material::AlphaMap>
{
  MeshLambertMaterial()
     : MaterialT(material::ResolverT<MeshLambertMaterial>::make(*this), material::Typer(this)) {}
  MeshLambertMaterial(const MeshLambertMaterial &material)
     : MaterialT(material, material::ResolverT<MeshLambertMaterial>::make(*this), material::Typer(this)) {}

protected:
  void callback(const material::Selector &selector) override;

public:
  using Ptr = std::shared_ptr<MeshLambertMaterial>;
  static Ptr make() {
    return Ptr(new MeshLambertMaterial());
  }

  MeshLambertMaterial *cloned() const override {
    return new MeshLambertMaterial(*this);
  }

  bool transparent() const override {return opacity < 1.0f;}
};

}
#endif //THREEPP_MESHLAMBERTMATERIAL_H
