//
// Created by byter on 13.09.17.
//

#ifndef THREEPP_MESHDEPTHMATERIAL_H
#define THREEPP_MESHDEPTHMATERIAL_H

#include "Material.h"
#include <threepp/textures/Texture.h>

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
struct MeshDepthMaterial : public MaterialT<material::Colored, material::AlphaMap, material::DisplacementMap>
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

protected:
  void callback(const material::Selector &selector) override;

public:
  using Ptr = std::shared_ptr<MeshDepthMaterial>;
  static Ptr make(DepthPacking packing, bool morphing, bool skinning) {
    return Ptr(new MeshDepthMaterial(packing, morphing, skinning));
  }
  static Ptr make(std::string name, DepthPacking packing, bool morphing, bool skinning) {
    Ptr p(new MeshDepthMaterial(packing, morphing, skinning));
    p->name = name;
    return p;
  }
};

}
#endif //THREEPP_MESHDEPTHMATERIAL_H
