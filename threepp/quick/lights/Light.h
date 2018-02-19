//
// Created by byter on 1/27/18.
//

#ifndef THREEPPQ_LIGHT_H
#define THREEPPQ_LIGHT_H

#include "threepp/quick/objects/ThreeQObject.h"

namespace three {
namespace quick {

class LightShadow;

class Light : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(bool helper READ helper WRITE setHelper NOTIFY helperChanged)

protected:
  QColor _color {Qt::white};

  float _intensity=1.0f;

  bool _helper = false;

  virtual three::Light::Ptr light() = 0;

  Light(QObject *parent) : ThreeQObject(parent) {}

  Light(three::Light::Ptr light, QObject *parent) : ThreeQObject(light, parent) {}

public:
  QColor color() const {return _color;}
  void setColor(const QColor &color) {
    if(_color != color) {
      _color = color;
      emit colorChanged();
    }
  }

  float intensity() const {return _intensity;}
  void setIntensity(float intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      if(light()) light()->intensity() = _intensity;
      emit intensityChanged();
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
  void helperChanged();
};

}
};


#endif //THREEPPQ_LIGHT_H
