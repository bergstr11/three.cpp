//
// Created by byter on 12/17/17.
//

#ifndef THREEPP_SPOTLIGHT_H
#define THREEPP_SPOTLIGHT_H

#include <light/SpotLight.h>
#include <quick/shadows/LightShadow.h>
#include "ThreeDObject.h"

namespace three {
namespace quick {

class SpotLight : public ThreeDObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(LightShadow * shadow READ shadow CONSTANT)

  QColor _color;
  LightShadow _shadow;

protected:
  Object3D::Ptr _create(ThreeDScene *scene) override
  {
    auto spot = three::SpotLight::make(scene->scene(), Color(_color.redF(), _color.greenF(), _color.blueF()));
    spot->shadow()->mapSize().x() = _shadow.mapSize().width();
    spot->shadow()->mapSize().y() = _shadow.mapSize().height();
    spot->shadow()->radius() = _shadow.radius();
    return spot;
  }

public:
  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  LightShadow *shadow() {return &_shadow;}

signals:
  void colorChanged();
};

}
}

#endif //THREEPP_SPOTLIGHT_H
