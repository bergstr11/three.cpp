//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_SPOTLIGHT_H
#define THREEQUICK_SPOTLIGHT_H

#include <light/SpotLight.h>
#include <quick/elements/LightShadow.h>
#include "ThreeDObject.h"

namespace three {
namespace quick {

class SpotLight : public ThreeDObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(LightShadow * shadow READ shadow CONSTANT)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
  Q_PROPERTY(qreal distance READ distance WRITE setDistance NOTIFY distanceChanged)
  Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
  Q_PROPERTY(qreal penumbra READ penumbra WRITE setPenumbra NOTIFY penumbraChanged)
  Q_PROPERTY(qreal decay READ decay WRITE setDecay NOTIFY decayChanged)

  QColor _color;
  LightShadow _shadow;

protected:
  Object3D::Ptr _create(ThreeDScene *scene) override
  {
    auto spot = three::SpotLight::make(scene->scene(), Color(_color.redF(), _color.greenF(), _color.blueF()));
    spot->shadow()->mapSize().x() = _shadow.mapSize().width();
    spot->shadow()->mapSize().y() = _shadow.mapSize().height();
    spot->shadow()->radius() = _shadow.radius();
    spot->intensity() = _intensity;
    spot->distance() = _distance;
    spot->angle() = _angle;
    spot->penumbra() = _penumbra;
    spot->decay() = _decay;
    return spot;
  }

  float _intensity=1, _distance=0, _angle=(float)M_PI / 3, _penumbra=0, _decay=1;

public:
  QColor color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  LightShadow *shadow() {return &_shadow;}

  qreal intensity() const {return _intensity;}
  void setIntensity(qreal intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      emit intensityChanged();
    }
  }

  qreal distance() const {return _distance;}
  void setDistance(qreal distance) {
    if(_distance != distance) {
      _distance = distance;
      emit distanceChanged();
    }
  }

  qreal angle() const {return _angle;}
  void setAngle(qreal angle) {
    if(_angle != angle) {
      _angle = angle;
      emit angleChanged();
    }
  }

  qreal penumbra() const {return _penumbra;}
  void setPenumbra(qreal penumbra) {
    if(_penumbra != penumbra) {
      _penumbra = penumbra;
      emit penumbraChanged();
    }
  }

  qreal decay() const {return _decay;}
  void setDecay(qreal decay) {
    if(_decay != decay) {
      _decay = decay;
      emit decayChanged();
    }
  }

signals:
  void colorChanged();
  void intensityChanged();
  void distanceChanged();
  void angleChanged();
  void penumbraChanged();
  void decayChanged();
};

}
}

#endif //THREEQUICK_SPOTLIGHT_H
