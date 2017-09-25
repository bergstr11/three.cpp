//
// Created by byter on 16.09.17.
//

#ifndef THREE_QT_SPOTLIGHTSHADOW_H
#define THREE_QT_SPOTLIGHTSHADOW_H

#include "LightShadow.h"
#include <camera/PerspectiveCamera.h>
#include <light/Light.h>

namespace three {

class SpotLightShadow : public CameraShadow<PerspectiveCamera>
{
  SpotLightShadow() : CameraShadow(PerspectiveCamera::make( 50, 1, 0.5, 500 )) {}

public:
  using Ptr = std::shared_ptr<SpotLightShadow>;
  static Ptr make() {return Ptr(new SpotLightShadow());}

  void update(Light::Ptr light) override
  {
    float fov = math::RAD2DEG * 2 * light->angle();
    float aspect = _mapSize.width() / _mapSize.height();
    float far = light->distance() || _camera->far();

    if ( fov != _camera->fov() || aspect != _camera->aspect() || far != _camera->far() ) {

      _camera->set(fov, aspect, far);
      _camera->updateProjectionMatrix();
    }
  }
};

}
#endif //THREE_QT_SPOTLIGHTSHADOW_H
