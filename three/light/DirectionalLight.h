//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_DIRECTIONALLIGHT_H
#define THREE_QT_DIRECTIONALLIGHT_H

#include "TargetLight.h"

namespace three {

class DirectionalLight : public TargetLight
{
protected:
  DirectionalLight(const Object3D::Ptr &target, const Color &color, float intensity)
     : TargetLight(light::ResolverT<DirectionalLight>::make(*this),  target, color, intensity)
  {}

public:
  using Ptr = std::shared_ptr<DirectionalLight>;

  static Ptr make(const Object3D::Ptr &target, const Color &color, float intensity) {
    return Ptr(new DirectionalLight(target, color, intensity));
  }
};

}
#endif //THREE_QT_DIRECTIONALLIGHT_H
