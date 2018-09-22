//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_SPOTLIGHT_H
#define THREEQUICK_SPOTLIGHT_H

#include <threepp/light/SpotLight.h>
#include <threepp/helper/SpotLight.h>
#include <threepp/quick/elements/LightShadow.h>
#include "Light.h"

namespace three {
namespace quick {

class SpotLight : public Light
{
Q_OBJECT
  Q_PROPERTY(LightShadowPC * shadow READ shadow CONSTANT)
  Q_PROPERTY(qreal distance READ distance WRITE setDistance NOTIFY distanceChanged)
  Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
  Q_PROPERTY(qreal penumbra READ penumbra WRITE setPenumbra NOTIFY penumbraChanged)
  Q_PROPERTY(qreal decay READ decay WRITE setDecay NOTIFY decayChanged)
  Q_PROPERTY(ThreeQObject *target READ target WRITE setTarget NOTIFY targetChanged)

  three::SpotLight::Ptr _light;
  helper::SpotLight::Ptr _helper;
  LightShadowPC _shadow;
  ThreeQObject *_target = nullptr;

protected:
  Object3D::Ptr _create() override
  {
    Object3D::Ptr target = _target ? _target->object() : _parentObject;
    _light = three::SpotLight::make(target, Color(_color.redF(), _color.greenF(), _color.blueF()));
    _light->intensity() = _intensity;
    _light->distance() = _distance;
    _light->angle() = _angle;
    _light->penumbra() = _penumbra;
    _light->decay() = _decay;
    _light->shadow()->mapSize().x() = _shadow.mapSize().width();
    _light->shadow()->mapSize().y() = _shadow.mapSize().height();
    _light->shadow()->radius() = _shadow.radius();
    _light->shadow()->bias() = _shadow.bias();
    _light->shadow()->camera()->setNearFar(_shadow.camera()->near(), _shadow.camera()->far());
    _light->shadow_t()->camera_t()->setFovAspect(_shadow.camera()->fov(), _shadow.camera()->aspect());

    return _light;
  }

  three::Light::Ptr light() override {return _light;}

  void _post_create() override
  {
    if(_qhelper.configured()) {
      _helper = helper::SpotLight::make("spotlight_helper", _light);
      QObject::connect(&_qhelper, &LightHelper::visibleChanged,
                       [&]() {_helper->visible() = _qhelper.visible();});
      _parentObject->add(_helper);
    }

    if(_shadow.camera()->helper()->configured()) {
      auto helper = _shadow.camera()->helper()->create(_light->shadow()->camera());
      _scene->scene()->add(helper);
    }
  }

  float _distance=0, _angle=(float)M_PI / 3, _penumbra=0, _decay=1;

public:
  SpotLight(QObject *parent = nullptr) : Light(parent) {}

  SpotLight(three::SpotLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {}

  LightShadowPC *shadow() {return &_shadow;}

  qreal distance() const {return _distance;}
  void setDistance(qreal distance) {
    if(_distance != distance) {
      _distance = distance;
      if(_light) _light->distance() = _distance;
      emit distanceChanged();
    }
  }

  qreal angle() const {return _angle;}
  void setAngle(qreal angle) {
    if(_angle != angle) {
      _angle = angle;
      if(_light) _light->angle() = _angle;
      emit angleChanged();
    }
  }

  qreal penumbra() const {return _penumbra;}
  void setPenumbra(qreal penumbra) {
    if(_penumbra != penumbra) {
      _penumbra = penumbra;
      if(_light) _light->penumbra() = _penumbra;
      emit penumbraChanged();
    }
  }

  qreal decay() const {return _decay;}
  void setDecay(qreal decay) {
    if(_decay != decay) {
      _decay = decay;
      if(_light) _light->decay() = _decay;
      emit decayChanged();
    }
  }

  ThreeQObject *target() const {return _target;}
  void setTarget(ThreeQObject *target) {
    if(_target != target) {
      _target = target;
      if(_light) _light->setTarget(target->object());
      emit targetChanged();
    }
  }

signals:
  void distanceChanged();
  void angleChanged();
  void penumbraChanged();
  void decayChanged();
  void targetChanged();
};

}
}

#endif //THREEQUICK_SPOTLIGHT_H
