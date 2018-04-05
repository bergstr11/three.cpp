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
#include "LightHelper.h"

namespace three {
namespace quick {

class HemisphereLight : public Light
{
Q_OBJECT
  Q_PROPERTY(QColor skyColor READ color WRITE setColor NOTIFY skyColorChanged)
  Q_PROPERTY(QColor groundColor READ groundColor WRITE setGroundColor NOTIFY groundColorChanged)

  QColor _groundColor {255, 255, 255};

  three::HemisphereLight::Ptr _light;
  helper::HemisphereLight::Ptr _helper;

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
    if(_qhelper.configured()) {
      _helper = helper::HemisphereLight::make(_light, _qhelper.size(), Color::null());
      _helper->visible() = _qhelper.visible();

      QObject::connect(&_qhelper, &LightHelper::visibleChanged,
                       [&]() {_helper->visible() = _qhelper.visible();});

      scene->scene()->add(_helper);
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

signals:
  void skyColorChanged();
  void groundColorChanged();
};

}
}

#endif //THREEQUICK_HEMISPHERELIGHT_H
