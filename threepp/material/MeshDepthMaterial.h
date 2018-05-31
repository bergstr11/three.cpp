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

  MeshDepthMaterial(DepthPacking packing)
     : MaterialT(material::InfoT<MeshDepthMaterial>(), material::Typer(this)), depthPacking(packing)
  {
  }

  MeshDepthMaterial(const MeshDepthMaterial &material)
     : MaterialT(material, material::InfoT<MeshDepthMaterial>(), material::Typer(this)),
       depthPacking(material.depthPacking)
  {}

protected:
  void callback(const material::Selector &selector) override;

public:
  using Ptr = std::shared_ptr<MeshDepthMaterial>;
  static Ptr make(DepthPacking packing, bool morphing, bool skinning) {
    Ptr p(new MeshDepthMaterial(packing));
    p->morphTargets = morphing;
    p->morphNormals = morphing;
    p->skinning = skinning;
    return p;
  }

  static Ptr make(std::string name, DepthPacking packing) {
    Ptr p(new MeshDepthMaterial(packing));
    p->name = name;
    return p;
  }

  MeshDepthMaterial *cloned() const override {
    return new MeshDepthMaterial(*this);
  }

  bool transparent() const override {return opacity < 1.0f;}
};

}
#endif //THREEPP_MESHDEPTHMATERIAL_H
