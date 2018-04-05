//
// Created by byter on 11/4/17.
//

#ifndef THREEPP_MESHPHONGMATERIAL_H
#define THREEPP_MESHPHONGMATERIAL_H

#include "Material.h"
#include <threepp/core/Color.h>

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author alteredq / http://alteredqualia.com/
 *
 * parameters = {
 *  color: <hex>,
 *  specular: <hex>,
 *  shininess: <float>,
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
 *  bumpMap: new THREE.Texture( <Image> ),
 *  bumpScale: <float>,
 *
 *  normalMap: new THREE.Texture( <Image> ),
 *  normalScale: <Vector2>,
 *
 *  displacementMap: new THREE.Texture( <Image> ),
 *  displacementScale: <float>,
 *  displacementBias: <float>,
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

struct MeshPhongMaterial : public MaterialT<
   material::Colored,
   material::LightMap,
   material::AoMap,
   material::Emissive,
   material::BumpMap,
   material::NormalMap,
   material::DisplacementMap,
   material::Specular,
   material::AlphaMap,
   material::EnvMap>
{
protected:
  MeshPhongMaterial(material::Resolver::Ptr resolver, const Color &color, bool dithering)
     : MaterialT(resolver)
  {
    this->color = color;
    this->dithering = dithering;
  }

  MeshPhongMaterial(material::Resolver::Ptr resolver)
     : MaterialT(resolver) {}

  MeshPhongMaterial(const Color &color, bool dithering)
     : MaterialT(material::ResolverT<MeshPhongMaterial>::make(*this))
  {
    this->color = color;
    this->dithering = dithering;
  }

  MeshPhongMaterial()
     : MaterialT(material::ResolverT<MeshPhongMaterial>::make(*this)) {}

  MeshPhongMaterial(const MeshPhongMaterial &material)
     : MaterialT(material, material::ResolverT<MeshPhongMaterial>::make(*this)) {}

  MeshPhongMaterial(const MeshPhongMaterial &material, material::Resolver::Ptr resolver)
     : MaterialT(material, resolver) {}

protected:
  void callback(const material::Selector &selector) override;

public:
  using Ptr = std::shared_ptr<MeshPhongMaterial>;

  static Ptr make(const Color &color, bool dithering) {
    return Ptr(new MeshPhongMaterial(color, dithering));
  }

  static Ptr make() {
    return Ptr(new MeshPhongMaterial());
  }

  MeshPhongMaterial *cloned() const override {
    return new MeshPhongMaterial(*this);
  }
};

}
#endif //THREEPP_MESHPHONGMATERIAL_H
