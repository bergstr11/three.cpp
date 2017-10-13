//
// Created by byter on 13.10.17.
//

#ifndef THREE_QT_SKINNEDMESH_H
#define THREE_QT_SKINNEDMESH_H

#include "Mesh.h"

namespace three {

class SkinnedMesh : public Mesh
{
  //BindMode bindMode = BindMode::attached;
  math::Matrix4 bindMatrix;
  math::Matrix4 bindMatrixInverse;

protected:
  SkinnedMesh(const Geometry::Ptr &geometry, const Material::Ptr &material)
     : Mesh(geometry, material) {}

public:
  using Ptr = std::shared_ptr<SkinnedMesh>;
  static Ptr make(const Geometry::Ptr &geometry, const Material::Ptr &material)
  {
    return std::shared_ptr<Mesh>(new SkinnedMesh(geometry, material));
  }

};

};
#endif //THREE_QT_SKINNEDMESH_H
