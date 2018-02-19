//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_DIRECTIONALLIGHT_H
#define THREEQUICK_DIRECTIONALLIGHT_H

#include <threepp/light/DirectionalLight.h>
#include <threepp/helper/DirectionalLight.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/elements/LightShadow.h>
#include "Light.h"

namespace three {
namespace quick {

class DirectionalLight : public Light
{
Q_OBJECT
  Q_PROPERTY(LightShadowOC * shadow READ shadow CONSTANT)
  Q_PROPERTY(ThreeQObject *target READ target WRITE setTarget NOTIFY targetChanged)
  Q_PROPERTY(int helperSize READ helperSize WRITE setHelperSize NOTIFY helperSizeChanged)

  three::DirectionalLight::Ptr _light;
  ThreeQObject *_target = nullptr;

  int _helperSize = 1;

  LightShadowOC _shadow;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    Object3D::Ptr target = _target ? _target->object() : scene->scene();
    _light = three::DirectionalLight::make(target,
                                           Color(_color.redF(), _color.greenF(), _color.blueF()),
                                           _intensity);

    _light->shadow()->mapSize().x() = _shadow.mapSize().width();
    _light->shadow()->mapSize().y() = _shadow.mapSize().height();
    _light->shadow()->radius() = _shadow.radius();
    _light->shadow()->camera()->setNearFar(_shadow.camera()->near(), _shadow.camera()->far());
    _light->shadow_t()->camera_t()->set(_shadow.camera()->left(),
                                        _shadow.camera()->right(),
                                        _shadow.camera()->top(),
                                        _shadow.camera()->bottom());

    return _light;
  }

  three::Light::Ptr light() override {return _light;}

  void _post_create(Scene *scene) override
  {
    if(_helper) {
      scene->scene()->add(helper::DirectionalLight::make(_light, _helperSize));
    }
  }

public:
  explicit DirectionalLight(QObject *parent = nullptr) : Light(parent) {}

  explicit DirectionalLight(three::DirectionalLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {}

  LightShadowOC *shadow() {return &_shadow;}

  ThreeQObject *target() {return _target;}

  void setTarget(ThreeQObject *target) {
    if(_target != target) {
      _target = target;
      emit targetChanged();
    }
  }

  int helperSize() {return _helperSize;}

  void setHelperSize(int helperSize) {
    if(_helperSize != helperSize) {
      _helperSize = helperSize;
      emit helperSizeChanged();
      setHelper(_helperSize > 0);
    }
  }

signals:
  void targetChanged();
  void helperSizeChanged();
};

}
}

#endif //THREEQUICK_DIRECTIONALLIGHT_H
