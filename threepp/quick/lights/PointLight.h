//
// Created by byter on 2/15/18.
//

#ifndef THREE_PP_POINTLIGHT_H
#define THREE_PP_POINTLIGHT_H

#include <threepp/light/PointLight.h>
#include <threepp/quick/lights/Light.h>

namespace three {
namespace quick {

class PointLight : public Light
{
  Q_OBJECT
  Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
  Q_PROPERTY(float decay READ decay WRITE setDecay NOTIFY decayChanged)

  float _decay = 1.0f;
  float _distance = 0.0f;

  three::PointLight::Ptr _light;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _light = three::PointLight::make(
       Color(_color.redF(), _color.greenF(), _color.blueF()), _intensity, _distance, _decay);
    return _light;
  }

  three::Light::Ptr light() override {return _light;}

public:
  PointLight(QObject *parent=nullptr) : Light(parent) {}
  PointLight(three::Light::Ptr light, QObject *parent) : Light(light, parent) {}

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

signals:
  void decayChanged();
  void distanceChanged();
};

}
}

#endif //THREE_PP_POINTLIGHT_H
