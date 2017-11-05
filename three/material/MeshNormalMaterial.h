//
// Created by byter on 11/4/17.
//

#ifndef THREE_QT_MESHNORMALMATERIAL_H
#define THREE_QT_MESHNORMALMATERIAL_H

#include "Material.h"

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author WestLangley / http://github.com/WestLangley
 *
 * parameters = {
 *  opacity: <float>,
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
 *  wireframe: <boolean>,
 *  wireframeLinewidth: <float>
 *
 *  skinning: <bool>,
 *  morphTargets: <bool>,
 *  morphNormals: <bool>
 * }
 */
struct MeshNormalMaterial: public Material
{
  Texture::Ptr bumpMap;
  float bumpScale = 1;

  Texture::Ptr normalMap;
  math::Vector2 normalScale {1, 1};

  Texture::Ptr displacementMap;
  float displacementScale = 1;
  float displacementBias = 0;

protected:
  MeshNormalMaterial() : Material(material::ResolverT<MeshNormalMaterial>::make(*this))
  {
    wireframe = false;
    wireframeLineWidth = 1;

    fog = false;
    lights = false;

    skinning = false;
    morphTargets = false;
    morphNormals = false;
  }

public:
  using Ptr = std::shared_ptr<MeshNormalMaterial>;
  static Ptr make() {
    return Ptr(new MeshNormalMaterial());
  }
};

}

#endif //THREE_QT_MESHNORMALMATERIAL_H
