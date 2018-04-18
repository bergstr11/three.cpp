//
// Created by byter on 24.09.17.
//

#ifndef THREEPP_DIRECTIONALLIGHT_H
#define THREEPP_DIRECTIONALLIGHT_H

#include <threepp/camera/OrthographicCamera.h>
#include "TargetLight.h"

namespace three {

using DirectionalLightShadow = LightShadowT<OrthographicCamera>;

class DirectionalLight : public TargetLight
{
protected:
  DirectionalLightShadow::Ptr _shadow;

  DirectionalLight(const Object3D::Ptr &target, const Color &color, float intensity)
     : TargetLight(target, color, intensity)
  {
    _position = math::Vector3( 0, 1, 0 );
    updateMatrix();

    _shadow = DirectionalLightShadow::make(OrthographicCamera::make(-5, 5, 5, -5, 0.5, 500));
  }

public:
  using Ptr = std::shared_ptr<DirectionalLight>;

  static Ptr make(const Object3D::Ptr &target, const Color &color, float intensity) {
    return Ptr(new DirectionalLight(target, color, intensity));
  }

  const LightShadow::Ptr shadow() const override {return _shadow;}
  const DirectionalLightShadow::Ptr shadow_t() const {return _shadow;}
};

}
#endif //THREEPP_DIRECTIONALLIGHT_H
