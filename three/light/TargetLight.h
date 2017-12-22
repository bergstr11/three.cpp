//
// Created by byter on 23.09.17.
//

#ifndef THREE_QT_TARGETLIGHT_H
#define THREE_QT_TARGETLIGHT_H

#include "Light.h"

namespace three {

class TargetLight : public Light
{
protected:
  Object3D::Ptr _target;

  TargetLight(light::Resolver::Ptr resolver, Object3D::Ptr target, const Color &color, float intensity)
     : Light(resolver, color, intensity), _target(target)
  {}

public:
  using Ptr = std::shared_ptr<TargetLight>;

  const Object3D::Ptr target() const {return _target;}
};

}
#endif //THREE_QT_TARGETLIGHT_H
