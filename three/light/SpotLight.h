//
// Created by byter on 16.09.17.
//

#ifndef THREE_QT_SPOTLIGHT_H
#define THREE_QT_SPOTLIGHT_H

#include "TargetLight.h"
#include "SpotLightShadow.h"

namespace three {

class SpotLight : public TargetLight
{
  float _penumbra;
  float _decay;
  Object3D::Ptr _target;

  SpotLight(Object3D::Ptr target,
            const Color &color,
            float intensity,
            float distance,
            float angle=(float)M_PI / 3.0f,
            float penumbra=0,
            float decay=1) // for physically correct lights, should be 2.
     : TargetLight(light::ResolverT<SpotLight>::make(*this),
                   target, color, intensity, distance, angle), _penumbra(penumbra), _decay(decay)
  {
    _position = math::Vector3( 0, 1, 0 );
    updateMatrix();

    _shadow = SpotLightShadow::make();
  }

public:
  using Ptr = std::shared_ptr<SpotLight>;
  static Ptr make(Object3D::Ptr target, const Color &color, float intensity, float distance, float angle, float penumbra, float decay) {
    return Ptr(new SpotLight(target, color, intensity, distance, angle, penumbra, decay));
  }

  float penumbra() const {return _penumbra;}

  float decay() const {return _decay;}

  double power() {
    return _intensity * M_PI;
  }

  void set_power(float power) {
    // intensity = power per solid angle.
    // ref: equation (17) from http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    _intensity = power / (float)M_PI;
  }
};

}
#endif //THREE_QT_SPOTLIGHT_H
