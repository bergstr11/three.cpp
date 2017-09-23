//
// Created by byter on 16.09.17.
//

#ifndef THREE_QT_SPOTLIGHT_H
#define THREE_QT_SPOTLIGHT_H

#include "Light.h"
#include "SpotLightShadow.h"

namespace three {

class SpotLight : public Light
{
  float _penumbra;
  float _decay;
  Object3D::Ptr _target;

  SpotLight(const Color &color,
            float intensity,
            float distance,
            float angle=(float)M_PI / 3.0f,
            float penumbra=0,
            float decay=1) // for physically correct lights, should be 2.
     : Light(color, intensity, distance, angle), _penumbra(penumbra), _decay(decay)
  {
    _position = math::Vector3( 0, 1, 0 );
    updateMatrix();

    _shadow = SpotLightShadow::make();
  }

public:
  using Ptr = std::shared_ptr<SpotLight>;
  static Ptr make(const Color &color, float intensity, float distance, float angle, float penumbra, float decay) {
    return new SpotLight(color, intensity, distance, angle, penumbra, decay);
  }

  double power() {
    return _intensity * M_PI;
  }

  void set_power(float power) {
    // intensity = power per solid angle.
    // ref: equation (17) from http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    _intensity = power / M_PI;
  }
};

}
#endif //THREE_QT_SPOTLIGHT_H
