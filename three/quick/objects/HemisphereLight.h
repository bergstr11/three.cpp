//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_HEMISPHERELIGHT_H
#define THREEQUICK_HEMISPHERELIGHT_H

#include <three/light/HemisphereLight.h>
#include <three/quick/elements/LightShadow.h>
#include <three/quick/scene/Scene.h>
#include "Light.h"

namespace three {
namespace quick {

class HemisphereLight : public Light
{
Q_OBJECT
  Q_PROPERTY(QColor skyColor READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor groundColor READ groundColor WRITE setGroundColor NOTIFY groundColorChanged)

  QColor _color {255, 255, 255}, _groundColor {255, 255, 255};

  three::HemisphereLight::Ptr _light;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _light = three::HemisphereLight::make(
       Color(_color.redF(), _color.greenF(), _color.blueF()),
       Color(_groundColor.redF(), _groundColor.greenF(), _groundColor.blueF()),
       _intensity);

    return _light;
  }

public:
  HemisphereLight(QObject *parent = nullptr) : Light(parent) {}

  HemisphereLight(three::HemisphereLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {}

  QColor color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  QColor groundColor() const {return _groundColor;}
  void setGroundColor(const QColor &color) {
    if(_groundColor != color) {
      _groundColor = color;
      emit groundColorChanged();
    }
  }

signals:
  void colorChanged();
  void groundColorChanged();
};

}
}

#endif //THREEQUICK_HEMISPHERELIGHT_H
