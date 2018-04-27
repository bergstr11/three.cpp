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

  three::DirectionalLight::Ptr _light;
  ThreeQObject *_target = nullptr;

  helper::DirectionalLight::Ptr _helper;

  LightShadowOC _shadow;

protected:
  Object3D::Ptr _create() override
  {
    Object3D::Ptr target = _target ? _target->object() : _parentObject;
    _light = three::DirectionalLight::make(target,
                                           Color(_color.redF(), _color.greenF(), _color.blueF()),
                                           _intensity);

    _light->shadow()->mapSize().x() = _shadow.mapSize().width();
    _light->shadow()->mapSize().y() = _shadow.mapSize().height();
    _light->shadow()->radius() = _shadow.radius();
    _light->shadow()->bias() = _shadow.bias();
    _light->shadow_t()->camera_t()->set(_shadow.camera()->left(),
                                        _shadow.camera()->right(),
                                        _shadow.camera()->top(),
                                        _shadow.camera()->bottom(),
                                        _shadow.camera()->near(),
                                        _shadow.camera()->far());

    return _light;
  }

  three::Light::Ptr light() override {return _light;}

  void _post_create() override
  {
    if(_qhelper.configured()) {
      _helper = helper::DirectionalLight::make(_light, _qhelper.size());
      _helper->visible() = _qhelper.visible();

      QObject::connect(&_qhelper, &LightHelper::visibleChanged,
                       [&]() {_helper->visible() = _qhelper.visible();});

      _parentObject->add(_helper);
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

signals:
  void targetChanged();
};

}
}

#endif //THREEQUICK_DIRECTIONALLIGHT_H
