//
// Created by byter on 23.09.17.
//

#ifndef THREEPP_TARGETLIGHT_H
#define THREEPP_TARGETLIGHT_H

#include "Light.h"

namespace three {

class TargetLight : public Light
{
protected:
  Object3D::Ptr _target;

  TargetLight(Object3D::Ptr target, const Color &color, float intensity, const object::Typer &typer)
     : Light(typer, color, intensity), _target(target)
  {
    Object3D::typer.allow<TargetLight>();
  }

public:
  using Ptr = std::shared_ptr<TargetLight>;

  const Object3D::Ptr target() const {return _target;}

  void setTarget(const Object3D::Ptr &target)
  {
    _target = target;
  }

  TargetLight *cloned() const override
  {
    //not easily done!
    return nullptr;
  }
};

}
#endif //THREEPP_TARGETLIGHT_H
