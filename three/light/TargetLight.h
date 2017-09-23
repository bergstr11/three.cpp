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

  TargetLight(Object3D::Ptr target, const Color &color, float intensity, float distance=0, float angle=0)
     : Light(color, intensity, distance, angle), _target(target)
  {}

public:
  using Ptr = std::shared_ptr<TargetLight>;

  const Object3D::Ptr target() const {return _target;}
};

}
#endif //THREE_QT_TARGETLIGHT_H
