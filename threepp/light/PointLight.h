//
// Created by byter on 23.09.17.
//

#ifndef THREEPP_POINTLIGHT_H
#define THREEPP_POINTLIGHT_H

#include <threepp/camera/PerspectiveCamera.h>
#include "Light.h"

namespace three {

using PointLightShadow = LightShadowT<PerspectiveCamera>;

class PointLight : public Light
{
  float _distance = 0;
  float _decay = 1;	// for physically correct lights, should be 2.

  PointLightShadow::Ptr _shadow;

protected:
  PointLight(const Color &color, float intensity, float distance, float decay)
     : Light(color, intensity), _distance(distance), _decay(decay)
  {
    _shadow = PointLightShadow::make(PerspectiveCamera::make( 90, 1, 0.5, 500));
  }

  PointLight(const PointLight &light)
     : Light(light), _distance(light.distance()), _decay(light.decay()), _shadow(light._shadow->cloned())
  { }

public:
  using Ptr = std::shared_ptr<PointLight>;
  static Ptr make(const Color &color, float intensity, float distance, float decay)
  {
    return Ptr(new PointLight(color, intensity, distance, decay));
  }

  const LightShadow::Ptr shadow() const override {return _shadow;}
  const PointLightShadow::Ptr shadow_t() const {return _shadow;}

  float distance() const {return _distance;}

  void setDistance(float distance) {
    _distance = distance;
  }

  float decay() const {return _decay;}

  void setDecay(float decay) {
    _decay = decay;
  }

  float power() const
  {
    // intensity = power per solid angle.
    // ref: equation (15) from http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    return _intensity * 4 * (float)M_PI;
  }

  void setPower(float power) {
    _intensity = power / ( 4 * (float)M_PI );
  }

  PointLight *cloned() const override
  {
    return new PointLight(*this);
  }
};

}
#endif //THREEPP_POINTLIGHT_H
