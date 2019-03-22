//
// Created by byter on 2/15/18.
//

#ifndef THREE_PP_POINTLIGHT_H
#define THREE_PP_POINTLIGHT_H

#include <threepp/light/PointLight.h>
#include <threepp/quick/lights/Light.h>
#include <threepp/quick/elements/LightShadow.h>

namespace three {
namespace quick {

class PointLight : public Light
{
  Q_OBJECT
  Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
  Q_PROPERTY(float decay READ decay WRITE setDecay NOTIFY decayChanged)
  Q_PROPERTY(float power READ power WRITE setPower NOTIFY powerChanged)
  Q_PROPERTY(LightShadowPC * shadow READ shadow CONSTANT)

  float _decay = 1.0f;
  float _distance = 0.0f;
  LightShadowPC _shadow;

  three::PointLight::Ptr _light;

protected:
  Object3D::Ptr _create() override
  {
    _light = three::PointLight::make(
       Color(_color.redF(), _color.greenF(), _color.blueF()), _intensity, _distance, _decay);
    _light->shadow()->mapSize().x() = _shadow.mapSize().width();
    _light->shadow()->mapSize().y() = _shadow.mapSize().height();
    _light->shadow()->radius() = _shadow.radius();
    _light->shadow()->bias() = _shadow.bias();
    _light->shadow()->camera()->setNearFar(_shadow.camera()->near(), _shadow.camera()->far());
    _light->shadow_t()->camera_t()->setFovAspect(_shadow.camera()->fov(), _shadow.camera()->aspect());
    return _light;
  }

  three::Light::Ptr light() override {return _light;}

public:
  PointLight(QObject *parent=nullptr) : Light(parent) {}
  PointLight(three::Light::Ptr light, QObject *parent) : Light(light, parent) {}

  LightShadowPC *shadow() {return &_shadow;}

  float distance() const {return _distance;}

  void setDistance(float distance) {
    if(_distance != distance) {
      _distance = distance;
      if(_light) _light->setDistance(_distance);
      emit distanceChanged();
    }
  }

  float decay() const {return _decay;}

  void setDecay(float decay) {
    if(_decay != decay) {
      _decay = decay;
      if(_light) _light->setDecay(_decay);
      emit decayChanged();
    }
  }

  float power() const {return _intensity * 4 * (float)M_PI;}

  void setPower(float power) {
    if(power != this->power()) {
      setIntensity(power / ( 4 * (float)M_PI ));
      emit powerChanged();
    }
  }

signals:
  void decayChanged();
  void distanceChanged();
  void powerChanged();
};

}
}

#endif //THREE_PP_POINTLIGHT_H
