//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_AMBIENTLIGHT_H
#define THREEQUICK_AMBIENTLIGHT_H

#include <light/AmbientLight.h>
#include <quick/elements/LightShadow.h>
#include "ThreeDObject.h"

namespace three {
namespace quick {

class AmbientLight : public ThreeDObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)

  QColor _color;
  qreal _intensity = 1.0f;

protected:
  Object3D::Ptr _create(ThreeDScene *scene) override
  {
    return three::AmbientLight::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _intensity);
  }

public:
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
