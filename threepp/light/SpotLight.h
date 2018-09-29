//
// Created by byter on 16.09.17.
//

#ifndef THREEPP_SPOTLIGHT_H
#define THREEPP_SPOTLIGHT_H

#include <threepp/camera/PerspectiveCamera.h>
#include "TargetLight.h"

namespace three {

class SpotLight;

class SpotLightShadow : public LightShadowT<PerspectiveCamera>
{
  SpotLightShadow(SpotLight &light)
     : LightShadowT(PerspectiveCamera::make( 50, 1, 0.5, 500 )), light(light) {}

  SpotLight &light;

public:
  using Ptr = std::shared_ptr<SpotLightShadow>;
  static Ptr make(SpotLight &light) {return Ptr(new SpotLightShadow(light));}

  void update() override;
};

class SpotLight : public TargetLight
{
  float _penumbra;
  float _decay;
  float _distance;
  float _angle;

  SpotLightShadow::Ptr _shadow;

  SpotLight(Object3D::Ptr target,
            const Color &color,
            float intensity,
            float distance,
            float angle,
            float penumbra,
            float decay) // for physically correct lights, should be 2.
     : TargetLight(target, color, intensity, object::Typer(this)),
       _distance(distance), _angle(angle), _penumbra(penumbra), _decay(decay)
  {
    _position = math::Vector3( 0, 1, 0 );
    updateMatrix();

    _shadow = SpotLightShadow::make(*this);
  }

public:
  using Ptr = std::shared_ptr<SpotLight>;
  static Ptr make(Object3D::Ptr target,
                  const Color &color,
                  float intensity=1,
                  float distance=0,
                  float angle=(float)M_PI / 3,
                  float penumbra=0,
                  float decay=1)
  {
    return Ptr(new SpotLight(target, color, intensity, distance, angle, penumbra, decay));
  }

  float distance() const {return _distance;}
  float &distance() {return _distance;}

  float angle() const {return _angle;}
  float &angle() {return _angle;}

  float penumbra() const {return _penumbra;}
  float &penumbra() {return _penumbra;}

  float decay() const {return _decay;}
  float &decay() {return _decay;}

  double power() {return _intensity * M_PI; }

  void setPower(float power) {
    // intensity = power per solid angle.
    // ref: equation (17) from http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    _intensity = power / (float)M_PI;
  }

  const LightShadow::Ptr shadow() const override {return _shadow;}
  SpotLightShadow::Ptr shadow_t() const {return _shadow;}
};

inline void SpotLightShadow::update()
{
  float fov = (float)math::RAD2DEG * 2 * light.angle();
  float aspect = _mapSize.width() / _mapSize.height();
  float far = light.distance() > 0 ? light.distance() : _camera->far();

  if ( fov != _camera->fov() || aspect != _camera->aspect() || far != _camera->far() ) {
    _camera->setFovAspect(fov, aspect);
    _camera->setFar(far);
    _camera->updateProjectionMatrix();
  }
}

}
#endif //THREEPP_SPOTLIGHT_H
