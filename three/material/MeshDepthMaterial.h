//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_MESHDEPTHMATERIAL_H
#define THREE_QT_MESHDEPTHMATERIAL_H

#include "Material.h"
#include <textures/Texture.h>

namespace three {

/**
 * @author mrdoob / http://mrdoob.com/
 * @author alteredq / http://alteredqualia.com/
 * @author bhouston / https://clara.io
 * @author WestLangley / http://github.com/WestLangley
 *
 * parameters = {
 *
 *  opacity: <float>,
 *
 *  map: new THREE.Texture( <Image> ),
 *
 *  alphaMap: new THREE.Texture( <Image> ),
 *
 *  displacementMap: new THREE.Texture( <Image> ),
 *  displacementScale: <float>,
 *  displacementBias: <float>,
 *
 *  wireframe: <boolean>,
 *  wireframeLinewidth: <float>
 * }
 */
struct MeshDepthMaterial : public MaterialT<material::AlphaMap, material::DisplacementMap>
{
  DepthPacking depthPacking = DepthPacking::Basic;

  bool skinning = false;

  bool fog = false;
  bool lights = false;

  MeshDepthMaterial(DepthPacking packing, bool morphing, bool skinning)
     : MaterialT(material::ResolverT<MeshDepthMaterial>::make(*this)), depthPacking(packing)
  {
    this->morphTargets = morphing;
    this->morphNormals = morphing;
    this->skinning = skinning;
  }

public:
  using Ptr = std::shared_ptr<MeshDepthMaterial>;
  static Ptr make(DepthPacking packing, bool morphing, bool skinning) {
    return Ptr(new MeshDepthMaterial(packing, morphing, skinning));
  }
};

}
#endif //THREE_QT_MESHDEPTHMATERIAL_H
