//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_DIRECTIONALLIGHT_H
#define THREEQUICK_DIRECTIONALLIGHT_H

#include <light/DirectionalLight.h>
#include <quick/scene/Scene.h>
#include <quick/elements/LightShadow.h>
#include "ThreeQObject.h"

namespace three {
namespace quick {

class DirectionalLight : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
  Q_PROPERTY(ThreeQObject *target READ target WRITE setTarget NOTIFY targetChanged)

  QColor _color;
  qreal _intensity = 1.0f;

  three::DirectionalLight::Ptr _light;
  ThreeQObject *_target = nullptr;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    Object3D::Ptr target = _target ? _target->object() : scene->scene();
    _light = three::DirectionalLight::make(target, Color(_color.redF(), _color.greenF(), _color.blueF()),
                                           _intensity);
    return _light;
  }

public:
  DirectionalLight(QObject *parent = nullptr) : ThreeQObject(parent) {}

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

  float intensity() {return _intensity;}

  void setIntensity(float intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      if(_light) {
        _light->intensity() = intensity;
      }
      emit intensityChanged();
    }
  }
signals:
  void colorChanged();
  void intensityChanged();
  void targetChanged();
};

}
}

#endif //THREEQUICK_DIRECTIONALLIGHT_H
