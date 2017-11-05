//
// Created by byter on 11/4/17.
//

#ifndef THREE_QT_MESHPHONGMATERIAL_H
#define THREE_QT_MESHPHONGMATERIAL_H

#include "Material.h"
#include <core/Color.h>

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

struct MeshPhongMaterial : public Material
{
  Color color = 0xffffff; // diffuse
  Color specular = 0x111111;
  float shininess = 30;

  Texture::Ptr lightMap;
  float lightMapIntensity = 1.0;

  Texture::Ptr aoMap;
  float aoMapIntensity = 1.0;

  Color emissive = 0x000000;
  float emissiveIntensity = 1.0;
  Texture::Ptr emissiveMap;

  Texture::Ptr bumpMap;
  float bumpScale = 1;

  Texture::Ptr normalMap;
  math::Vector2 normalScale {1, 1};

  Texture::Ptr displacementMap;
  float displacementScale = 1;
  float displacementBias = 0;

  Texture::Ptr specularMap;

  Texture::Ptr alphaMap;

  CombineOperation combine = CombineOperation::Multiply;
  float reflectivity = 1;
  float refractionRatio = 0.98;

protected:
  MeshPhongMaterial(material::Resolver::Ptr resolver) : Material(resolver) {}
  MeshPhongMaterial() : Material(material::ResolverT<MeshPhongMaterial>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<MeshPhongMaterial>;
  static Ptr make() {
    return Ptr(new MeshPhongMaterial());
  }
};

}
#endif //THREE_QT_MESHPHONGMATERIAL_H
