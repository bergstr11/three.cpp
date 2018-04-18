//
// Created by byter on 13.10.17.
//

#ifndef THREEPP_SKINNEDMESH_H
#define THREEPP_SKINNEDMESH_H

#include "Mesh.h"
#include "Skeleton.h"

namespace three {

class SkinnedMesh : public MeshT<BufferGeometry, Material>
{
  //BindMode bindMode = BindMode::attached;
  math::Matrix4 _bindMatrix;
  math::Matrix4 _bindMatrixInverse;

  Skeleton::Ptr _skeleton;

protected:
  SkinnedMesh(const BufferGeometry::Ptr &geometry, const Material::Ptr &material)
     : Object3D(), MeshT(geometry, material) {}

public:
  using Ptr = std::shared_ptr<SkinnedMesh>;
  static Ptr make(const BufferGeometry::Ptr &geometry, const Material::Ptr &material)
  {
    return Ptr(new SkinnedMesh(geometry, material));
  }

  const Skeleton::Ptr skeleton() const {return _skeleton;}

  const math::Matrix4 &bindMatrix() const {return _bindMatrix;}
  const math::Matrix4 &bindMatrixInverse() const {return _bindMatrixInverse;}
};

};
#endif //THREEPP_SKINNEDMESH_H
