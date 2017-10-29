//
// Created by byter on 26.10.17.
//

#ifndef THREE_QT_IMMEDIATERENDEROBJECT_H
#define THREE_QT_IMMEDIATERENDEROBJECT_H

#include <core/Object3D.h>

namespace three {

class ImmediateRenderObject : public Object3DBase<Material>
{
  explicit ImmediateRenderObject(Material::Ptr material)
     : Object3DBase<Material>(object::ResolverT<ImmediateRenderObject>::make(*this), material) {}

public:
  using Ptr = std::shared_ptr<ImmediateRenderObject>;
  static Ptr make(Material::Ptr material) {
    return Ptr(new ImmediateRenderObject(material));
  }
};

}
#endif //THREE_QT_IMMEDIATERENDEROBJECT_H
