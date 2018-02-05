//
// Created by byter on 12/20/17.
//

#ifndef THREEPPQ_FOG_H
#define THREEPPQ_FOG_H

#include <QObject>
#include <QColor>
#include <threepp/scene/Fog.h>

namespace three {
namespace quick {

class FogBase : public QObject
{
Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

protected:
  QColor _color {0xcccccc};

public:
  virtual ~FogBase() {}

  QColor color() const {return _color;}

  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  virtual three::Fog::Ptr create() const = 0;

signals:
  void colorChanged();
};

class Fog : public FogBase
{
Q_OBJECT
  Q_PROPERTY(qreal near READ near WRITE setNear NOTIFY nearChanged)
  Q_PROPERTY(qreal far READ far WRITE setFar NOTIFY farChanged)

  float _near=0.02, _far=100;

public:
  float near() const {return _near;}
  float far() const {return _far;}

  void setNear(float near) {
    if(_near != near) {
      _near = near;
      emit nearChanged();
    }
  }

  void setFar(float far) {
    if(_far != far) {
      _far = far;
      emit farChanged();
    }
  }

  three::Fog::Ptr create() const override {
    return three::DefaultFog::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _near, _far);
  }

signals:
  void nearChanged();
  void farChanged();
};

class FogExp2 : public FogBase
{
Q_OBJECT
  Q_PROPERTY(qreal density READ density WRITE setDensity NOTIFY densityChanged)

  float _density = 0.00025f;

public:
  float density() const {return _density;}

  void setDensity(float density) {
    if(_density != density) {
      _density = density;
      emit densityChanged();
    }
  }

  three::Fog::Ptr create() const override {
    return three::FogExp2::make(Color(_color.redF(), _color.greenF(), _color.blueF()), _density);
  }

signals:
  void densityChanged();
};

}
}

#endif //THREEPPQ_FOG_H
