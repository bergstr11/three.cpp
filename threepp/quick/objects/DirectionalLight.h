//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_DIRECTIONALLIGHT_H
#define THREEQUICK_DIRECTIONALLIGHT_H

#include <threepp/light/DirectionalLight.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/elements/LightShadow.h>
#include "Light.h"

namespace three {
namespace quick {

class DirectionalLight : public Light
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(ThreeQObject *target READ target WRITE setTarget NOTIFY targetChanged)

  QColor _color {255, 255, 255};

  three::DirectionalLight::Ptr _light;
  ThreeQObject *_target = nullptr;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    Object3D::Ptr target = _target ? _target->object() : scene->scene();
    _light = three::DirectionalLight::make(target,
                                           Color(_color.redF(), _color.greenF(), _color.blueF()),
                                           _intensity);
    return _light;
  }

public:
  DirectionalLight(QObject *parent = nullptr) : Light(parent) {}

  DirectionalLight(three::DirectionalLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  ThreeQObject *target() {return _target;}

  void setTarget(ThreeQObject *target) {
    if(_target != target) {
      _target = target;
      emit targetChanged();
    }
  }

signals:
  void colorChanged();
  void targetChanged();
};

}
}

#endif //THREEQUICK_DIRECTIONALLIGHT_H
