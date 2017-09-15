//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_MESHDEPTHMATERIAL_H
#define THREE_QT_MESHDEPTHMATERIAL_H

#include "Material.h"
#include <textures/Texture.h>

namespace three {

class MeshDepthMaterial : public Material
{
  DepthPacking _depthPacking = DepthPacking::Basic;

  bool _skinning = false;

  Texture::Ptr _map;

  Texture::Ptr _alphaMap;

  Texture::Ptr _displacementMap;

  float _displacementScale = 1;
  float _displacementBias = 0;

  bool _wireframe = false;
  float _wireframeLinewidth = 1;

  bool _fog = false;
  bool _lights = false;

  MeshDepthMaterial(DepthPacking packing, bool morphing, bool skinning)
     : Material(morphing, skinning), _depthPacking(packing) {}
public:
  using Ptr = std::shared_ptr<MeshDepthMaterial>;
  static Ptr make(DepthPacking packing, bool morphing, bool skinning) {
    return std::shared_ptr<MeshDepthMaterial>(new MeshDepthMaterial(packing, morphing, skinning));
  }
};

}
#endif //THREE_QT_MESHDEPTHMATERIAL_H
