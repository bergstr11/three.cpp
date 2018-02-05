//
// Created by byter on 26.10.17.
//

#ifndef THREEPP_IMMEDIATERENDEROBJECT_H
#define THREEPP_IMMEDIATERENDEROBJECT_H

#include <threepp/core/Object3D.h>

namespace three {

class ImmediateRenderObject : public Object3D_GM<Material>
{
  explicit ImmediateRenderObject(Material::Ptr material)
     : Object3D_GM<Material>(object::ResolverT<ImmediateRenderObject>::make(*this), material) {}

public:
  using Ptr = std::shared_ptr<ImmediateRenderObject>;
  static Ptr make(Material::Ptr material) {
    return Ptr(new ImmediateRenderObject(material));
  }
};

}
#endif //THREEPP_IMMEDIATERENDEROBJECT_H
