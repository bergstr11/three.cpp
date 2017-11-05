//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_MESHDEPTHMATERIAL_H
#define THREE_QT_MESHDEPTHMATERIAL_H

#include "Material.h"
#include <textures/Texture.h>

namespace three {

struct MeshDepthMaterial : public Material
{
  DepthPacking depthPacking = DepthPacking::Basic;

  bool skinning = false;

  Texture::Ptr alphaMap;

  Texture::Ptr displacementMap;

  float displacementScale = 1;
  float displacementBias = 0;

  bool wireframe = false;
  float wireframeLinewidth = 1;

  bool fog = false;
  bool lights = false;

  MeshDepthMaterial(DepthPacking packing, bool morphing, bool skinning)
     : Material(material::ResolverT<MeshDepthMaterial>::make(*this)), depthPacking(packing)
  {
    this->morphTargets = morphing;
    this->morphNormals = morphing;
    this->skinning = skinning;
  }

public:
  using Ptr = std::shared_ptr<MeshDepthMaterial>;
  static Ptr make(DepthPacking packing, bool morphing, bool skinning) {
    return std::shared_ptr<MeshDepthMaterial>(new MeshDepthMaterial(packing, morphing, skinning));
  }
};

}
#endif //THREE_QT_MESHDEPTHMATERIAL_H
