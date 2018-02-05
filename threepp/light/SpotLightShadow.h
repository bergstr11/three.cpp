//
// Created by byter on 16.09.17.
//

#ifndef THREEPP_SPOTLIGHTSHADOW_H
#define THREEPP_SPOTLIGHTSHADOW_H

#include "LightShadow.h"
#include <threepp/camera/PerspectiveCamera.h>

namespace three {

class SpotLight;

class SpotLightShadow : public LightShadow
{
  SpotLightShadow(SpotLight &light)
     : LightShadow(PerspectiveCamera::make( 50, 1, 0.5, 500 )), light(light) {}

  SpotLight &light;

public:
  using Ptr = std::shared_ptr<SpotLightShadow>;
  static Ptr make(SpotLight &light) {return Ptr(new SpotLightShadow(light));}

  void update() override;
};

}
#endif //THREEPP_SPOTLIGHTSHADOW_H
