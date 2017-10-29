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
  DirectionalLight(const Ptr &target, const Color &color, float intensity, float distance, float angle)
     : TargetLight(light::ResolverT<DirectionalLight>::make(*this),  target, color, intensity, distance, angle)
  {}

public:
  using Ptr = std::shared_ptr<DirectionalLight>;

  bool castShadow() override
  {
    return false;
  }

  static Ptr make(const Ptr &target, const Color &color, float intensity, float distance, float angle) {
    return Ptr(new DirectionalLight(target, color, intensity, distance, angle));
  }
};

}
#endif //THREE_QT_DIRECTIONALLIGHT_H
