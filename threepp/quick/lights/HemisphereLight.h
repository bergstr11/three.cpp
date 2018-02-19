//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_HEMISPHERELIGHT_H
#define THREEQUICK_HEMISPHERELIGHT_H

#include <threepp/light/HemisphereLight.h>
#include <threepp/helper/HemisphereLight.h>
#include <threepp/quick/elements/LightShadow.h>
#include <threepp/quick/scene/Scene.h>
#include "Light.h"

namespace three {
namespace quick {

class HemisphereLight : public Light
{
Q_OBJECT
  Q_PROPERTY(QColor skyColor READ color WRITE setColor NOTIFY skyColorChanged)
  Q_PROPERTY(QColor groundColor READ groundColor WRITE setGroundColor NOTIFY groundColorChanged)
  Q_PROPERTY(float helperSize READ helperSize WRITE setHelperSize NOTIFY helperSizeChanged)

  QColor _groundColor {255, 255, 255};

  three::HemisphereLight::Ptr _light;

  float _helperSize = 1;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _light = three::HemisphereLight::make(
       Color(_color.redF(), _color.greenF(), _color.blueF()),
       Color(_groundColor.redF(), _groundColor.greenF(), _groundColor.blueF()),
       _intensity);

    return _light;
  }

  three::Light::Ptr light() override {return _light;}

  void _post_create(Scene *scene) override
  {
    if(_helper) {
      scene->scene()->add(helper::HemisphereLight::make(_light, _helperSize, Color(ColorName::blueviolet)));
    }
  }

public:
  HemisphereLight(QObject *parent = nullptr) : Light(parent) {
    QObject::connect(this, &Light::colorChanged, this, &HemisphereLight::skyColorChanged);
  }

  HemisphereLight(three::HemisphereLight::Ptr light, QObject *parent = nullptr)
     : Light(light, parent), _light(light) {
    QObject::connect(this, &Light::colorChanged, this, &HemisphereLight::skyColorChanged);
  }

  QColor groundColor() const {return _groundColor;}
  void setGroundColor(const QColor &color) {
    if(_groundColor != color) {
      _groundColor = color;
      emit groundColorChanged();
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
  void skyColorChanged();
  void groundColorChanged();
  void helperSizeChanged();
};

}
}

#endif //THREEQUICK_HEMISPHERELIGHT_H
