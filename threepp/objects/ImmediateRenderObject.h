//
// Created by byter on 26.10.17.
//

#ifndef THREEPP_IMMEDIATERENDEROBJECT_H
#define THREEPP_IMMEDIATERENDEROBJECT_H

#include <threepp/core/Object3D.h>

namespace three {

class ImmediateRenderObject : public Object3D_GM<BufferGeometry, Material>
{
  explicit ImmediateRenderObject(BufferGeometry::Ptr geom, Material::Ptr material)
     : Object3D_GM(geom, material) {}

  explicit ImmediateRenderObject(const ImmediateRenderObject &obj)
     : Object3D_GM(obj) {}

public:
  using Ptr = std::shared_ptr<ImmediateRenderObject>;
  static Ptr make(BufferGeometry::Ptr geom, Material::Ptr material) {
    return Ptr(new ImmediateRenderObject(geom, material));
  }

  Object3D *cloned() const override
  {
    return new ImmediateRenderObject(*this);
  }
};

}
#endif //THREEPP_IMMEDIATERENDEROBJECT_H
