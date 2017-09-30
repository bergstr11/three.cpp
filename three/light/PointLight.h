//
// Created by byter on 23.09.17.
//

#ifndef THREE_QT_POINTLIGHT_H
#define THREE_QT_POINTLIGHT_H

#include <camera/PerspectiveCamera.h>
#include "Light.h"

namespace three {

class PointLight : public Light
{
  float _distance = 0;
  float _decay = 1;	// for physically correct lights, should be 2.

protected:
  PointLight(const Color &color, float intensity, float distance, float angle)
     : Light(light::Resolver<PointLight>::make(this), color, intensity, distance, angle)
  {
    _shadow = CameraShadow<PerspectiveCamera>::make(PerspectiveCamera::make( 90, 1, 0.5, 500));
  }

public:
  using Ptr = std::shared_ptr<PointLight>;
  static Ptr make(const Color &color, float intensity, float distance, float angle)
  {
    return Ptr(new PointLight(color, intensity, distance, angle));
  }

  CameraShadow<PerspectiveCamera>::Ptr shadow()
  {
    return std::dynamic_pointer_cast<CameraShadow<PerspectiveCamera>>(_shadow);
  }

  bool castShadow() override
  {
    return true;
  }

  float decay() const {return _decay;}

  float power() const
  {
    // intensity = power per solid angle.
    // ref: equation (15) from http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    return _intensity * 4 * (float)M_PI;
  }

  void setPower(float power) {
    _intensity = power / ( 4 * (float)M_PI );
  }
};

}
#endif //THREE_QT_POINTLIGHT_H
