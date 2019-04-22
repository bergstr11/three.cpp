//
// Created by byter on 29.07.17.
//

#ifndef THREEPP_MESHBASICMATERIAL
#define THREEPP_MESHBASICMATERIAL

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
 *  map: new THREE.Texture( <Image> ),
 *
 *  lightMap: new THREE.Texture( <Image> ),
 *  lightMapIntensity: <float>
 *
 *  aoMap: new THREE.Texture( <Image> ),
 *  aoMapIntensity: <float>
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
 *  depthTest: <bool>,
 *  depthWrite: <bool>,
 *
 *  wireframe: <boolean>,
 *  wireframeLinewidth: <float>,
 *
 *  skinning: <bool>,
 *  morphTargets: <bool>
 * }
 */
struct DLX MeshBasicMaterial : public MaterialT<
   material::Diffuse,
   material::LightMap,
   material::AoMap,
   material::EnvMap,
   material::AlphaMap,
   material::SpecularMap>
{
private:
  MeshBasicMaterial()
     : MaterialT(material::InfoT<MeshBasicMaterial>(), material::Typer(this))
  {}

  MeshBasicMaterial(const MeshBasicMaterial &material)
     : MaterialT(*this, material::InfoT<MeshBasicMaterial>(), material::Typer(this))
  {}

public:
  using Ptr = std::shared_ptr<MeshBasicMaterial>;

  static Ptr make() {
    return Ptr(new MeshBasicMaterial());
  }

  MeshBasicMaterial *cloned() const override {
    return new MeshBasicMaterial(*this);
  }

  bool transparent() const override {return opacity < 1.0f;}
};

}


#endif //THREEPP_MESHBASICMATERIAL
