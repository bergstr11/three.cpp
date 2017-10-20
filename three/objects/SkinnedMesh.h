//
// Created by byter on 13.10.17.
//

#ifndef THREE_QT_SKINNEDMESH_H
#define THREE_QT_SKINNEDMESH_H

#include "Mesh.h"
#include "Skeleton.h"

namespace three {

class SkinnedMesh : public MeshBase<Material>
{
  //BindMode bindMode = BindMode::attached;
  math::Matrix4 bindMatrix;
  math::Matrix4 bindMatrixInverse;

  Skeleton _skeleton;

protected:
  SkinnedMesh(const Geometry::Ptr &geometry, const Material::Ptr &material)
     : MeshBase(geometry, material) {}

public:
  using Ptr = std::shared_ptr<SkinnedMesh>;
  static Ptr make(const Geometry::Ptr &geometry, const Material::Ptr &material)
  {
    return Ptr(new SkinnedMesh(geometry, material));
  }

  const Skeleton &skeleton() const {return _skeleton;}
};

};
#endif //THREE_QT_SKINNEDMESH_H
