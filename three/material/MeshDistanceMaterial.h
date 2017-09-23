//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_MESHDISTANCEMATERIAL_H
#define THREE_QT_MESHDISTANCEMATERIAL_H

#include "Material.h"
#include <math/Vector3.h>
#include <textures/Texture.h>

namespace three {

class MeshDistanceMaterial : public Material
{
  math::Vector3 _referencePosition;
  float _nearDistance = 1;
  float _farDistance = 1000;

  MeshDistanceMaterial(bool morphing, bool skinning) : Material(morphing, skinning) {}

public:
  using Ptr = std::shared_ptr<MeshDistanceMaterial>;
  static Ptr make(bool morphing, bool skinning) {return Ptr(new MeshDistanceMaterial(morphing, skinning));}
};

}
#endif //THREE_QT_MESHDISTANCEMATERIAL_H
