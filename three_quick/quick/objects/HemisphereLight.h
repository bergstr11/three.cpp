//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_HEMISPHERELIGHT_H
#define THREEQUICK_HEMISPHERELIGHT_H

#include <light/HemisphereLight.h>
#include <quick/elements/LightShadow.h>
#include <quick/scene/Scene.h>
#include "ThreeQObject.h"

namespace three {
namespace quick {

class HemisphereLight : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(QColor skyColor READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor groundColor READ groundColor WRITE setGroundColor NOTIFY groundColorChanged)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)

  QColor _color {255, 255, 255}, _groundColor {255, 255, 255};
  float _intensity = 1.0f;

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
  HemisphereLight(QObject *parent = nullptr) : ThreeQObject(parent) {}

  HemisphereLight(three::HemisphereLight::Ptr light, QObject *parent = nullptr)
     : ThreeQObject(light, parent), _light(light) {}

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

  qreal intensity() const {return _intensity;}
  void setIntensity(qreal intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      if(_light) _light->intensity() = _intensity;
      emit intensityChanged();
    }
  }

signals:
  void colorChanged();
  void groundColorChanged();
  void intensityChanged();
};

}
}

#endif //THREEQUICK_HEMISPHERELIGHT_H
