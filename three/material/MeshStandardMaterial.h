//
// Created by byter on 11/4/17.
//

#ifndef THREE_QT_MESHSTANDARDMATERIAL_H
#define THREE_QT_MESHSTANDARDMATERIAL_H

#include "Material.h"
#include <core/Color.h>

namespace three {

/**
 * @author WestLangley / http://github.com/WestLangley
 *
 * parameters = {
 *  color: <hex>,
 *  roughness: <float>,
 *  metalness: <float>,
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
 *  roughnessMap: new THREE.Texture( <Image> ),
 *
 *  metalnessMap: new THREE.Texture( <Image> ),
 *
 *  alphaMap: new THREE.Texture( <Image> ),
 *
 *  envMap: new THREE.CubeTexture( [posx, negx, posy, negy, posz, negz] ),
 *  envMapIntensity: <float>
 *
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

struct MeshStandardMaterial : public Material
{
  std::unordered_map<std::string, std::string> defines {{ "STANDARD", "" }};

  Color color = 0xffffff; // diffuse
  float roughness = 0.5;
  float metalness = 0.5;

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
  math::Vector2 normalScale { 1, 1 };

  Texture::Ptr displacementMap;
  float displacementScale = 1;
  float displacementBias = 0;

  Texture::Ptr roughnessMap;

  Texture::Ptr metalnessMap;

  Texture::Ptr alphaMap;

  float envMapIntensity = 1.0;

  float refractionRatio = 0.98;

protected:
  MeshStandardMaterial(material::Resolver::Ptr resolver) : Material(resolver) {}
  MeshStandardMaterial() : Material(material::ResolverT<MeshStandardMaterial>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<MeshStandardMaterial>;
  static Ptr make() {
    return Ptr(new MeshStandardMaterial());
  }
};

}

#endif //THREE_QT_MESHSTANDARDMATERIAL_H
