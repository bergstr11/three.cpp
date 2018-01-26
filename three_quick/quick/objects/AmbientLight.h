//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_AMBIENTLIGHT_H
#define THREEQUICK_AMBIENTLIGHT_H

#include <light/AmbientLight.h>
#include <quick/scene/Scene.h>
#include <quick/elements/LightShadow.h>
#include "ThreeQObject.h"

namespace three {
namespace quick {

class AmbientLight : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)

  QColor _color;
  qreal _intensity = 1.0f;

  three::AmbientLight::Ptr _ambient;
  Scene *_scene = nullptr;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _scene = scene;
    _ambient = three::AmbientLight::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _intensity);
    return _ambient;
  }

public:
  AmbientLight(QObject *parent = nullptr) : ThreeQObject(parent) {}

  AmbientLight(three::AmbientLight::Ptr light, QObject *parent = nullptr)
     : ThreeQObject(light, parent), _ambient(light) {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  float intensity() {return _intensity;}

  void setIntensity(float intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      if(_ambient) {
        _ambient->intensity() = intensity;
        _scene->scene()->updateMaterials();
      }
      emit intensityChanged();
    }
  }
signals:
  void colorChanged();
  void intensityChanged();
};

}
}

#endif //THREEQUICK_AMBIENTLIGHT_H
