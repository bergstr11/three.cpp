//
// Created by byter on 12/17/17.
//

#ifndef THREEQUICK_SPOTLIGHT_H
#define THREEQUICK_SPOTLIGHT_H

#include <light/SpotLight.h>
#include <helper/SpotLight.h>
#include <quick/elements/LightShadow.h>
#include "ThreeQObject.h"

namespace three {
namespace quick {

class SpotLight : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(LightShadow * shadow READ shadow CONSTANT)
  Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
  Q_PROPERTY(qreal distance READ distance WRITE setDistance NOTIFY distanceChanged)
  Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
  Q_PROPERTY(qreal penumbra READ penumbra WRITE setPenumbra NOTIFY penumbraChanged)
  Q_PROPERTY(qreal decay READ decay WRITE setDecay NOTIFY decayChanged)
  Q_PROPERTY(bool addHelper READ helper WRITE setHelper NOTIFY helperChanged)

  QColor _color;
  LightShadow _shadow;

  three::SpotLight::Ptr _spot;

protected:
  Object3D::Ptr _create(Scene *scene) override
  {
    _spot = three::SpotLight::make(scene->scene(), Color(_color.redF(), _color.greenF(), _color.blueF()));
    _spot->shadow()->mapSize().x() = _shadow.mapSize().width();
    _spot->shadow()->mapSize().y() = _shadow.mapSize().height();
    _spot->shadow()->radius() = _shadow.radius();
    _spot->intensity() = _intensity;
    _spot->distance() = _distance;
    _spot->angle() = _angle;
    _spot->penumbra() = _penumbra;
    _spot->decay() = _decay;
    _spot->shadow()->camera()->setNearFar(_shadow.camera()->near(), _shadow.camera()->far());
    _spot->shadow()->camera()->setFovAspect(_shadow.camera()->fov(), _shadow.camera()->aspect());

    return _spot;
  }

  void _post_create(Scene *scene) override
  {
    if(_helper) {
      scene->scene()->add(helper::SpotLight::make("spotlight_helper", _spot));
    }
  }

  float _intensity=1, _distance=0, _angle=(float)M_PI / 3, _penumbra=0, _decay=1;

  bool _helper = false;

public:
  SpotLight(QObject *parent = nullptr) : ThreeQObject(parent) {}

  SpotLight(three::SpotLight::Ptr light, QObject *parent = nullptr)
     : ThreeQObject(light, parent), _spot(light) {}

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
      if(_spot) _spot->intensity() = _intensity;
      emit intensityChanged();
    }
  }

  qreal distance() const {return _distance;}
  void setDistance(qreal distance) {
    if(_distance != distance) {
      _distance = distance;
      if(_spot) _spot->distance() = _distance;
      emit distanceChanged();
    }
  }

  qreal angle() const {return _angle;}
  void setAngle(qreal angle) {
    if(_angle != angle) {
      _angle = angle;
      if(_spot) _spot->angle() = _angle;
      emit angleChanged();
    }
  }

  qreal penumbra() const {return _penumbra;}
  void setPenumbra(qreal penumbra) {
    if(_penumbra != penumbra) {
      _penumbra = penumbra;
      if(_spot) _spot->penumbra() = _penumbra;
      emit penumbraChanged();
    }
  }

  qreal decay() const {return _decay;}
  void setDecay(qreal decay) {
    if(_decay != decay) {
      _decay = decay;
      if(_spot) _spot->decay() = _decay;
      emit decayChanged();
    }
  }

  bool helper() const {return _helper;}
  void setHelper(bool helper) {
    if(_helper != helper) {
      _helper = helper;
      emit helperChanged();
    }
  }

signals:
  void colorChanged();
  void intensityChanged();
  void distanceChanged();
  void angleChanged();
  void penumbraChanged();
  void decayChanged();
  void helperChanged();
};

}
}

#endif //THREEQUICK_SPOTLIGHT_H
