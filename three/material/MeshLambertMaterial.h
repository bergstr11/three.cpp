//
// Created by byter on 11/4/17.
//

#ifndef THREE_QT_MESHLAMBERTMATERIAL_H
#define THREE_QT_MESHLAMBERTMATERIAL_H

#include <core/Color.h>
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

struct MeshLambertMaterial : public Material
{
  Color color = 0xffffff; // diffuse


  Texture::Ptr lightMap;
  float lightMapIntensity = 1.0;

  Texture::Ptr aoMap;
  float aoMapIntensity = 1.0;

  Texture::Ptr emissiveMap;
  Color emissive = 0x000000;
  float emissiveIntensity = 1.0;

  Texture::Ptr specularMap;

  Texture::Ptr alphaMap;

  CombineOperation combine = CombineOperation::Multiply;
  unsigned reflectivity = 1;
  float refractionRatio = 0.98;

  MeshLambertMaterial() : Material(material::ResolverT<MeshLambertMaterial>::make(*this)) {}

public:
  using Ptr = std::shared_ptr<MeshLambertMaterial>;
  static Ptr make() {
    return Ptr(new MeshLambertMaterial());
  }
};

}
#endif //THREE_QT_MESHLAMBERTMATERIAL_H
