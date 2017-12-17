//
// Created by byter on 12/17/17.
//

#ifndef THREEPP_SPOTLIGHT_H
#define THREEPP_SPOTLIGHT_H

#include <light/SpotLight.h>
#include "ThreeDObject.h"

namespace three {
namespace quick {

class SpotLight : public ThreeDObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

  QColor _color;

protected:
  Object3D::Ptr _create(ThreeDScene *scene) override
  {
    return three::SpotLight::make(scene->scene(), Color(_color.redF(), _color.greenF(), _color.blueF()));
  }

public:
  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

signals:
  void colorChanged();
};

}
}

#endif //THREEPP_SPOTLIGHT_H
