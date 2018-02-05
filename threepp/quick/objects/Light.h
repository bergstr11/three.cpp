//
// Created by byter on 1/27/18.
//

#ifndef THREEPPQ_LIGHT_H
#define THREEPPQ_LIGHT_H

#include "ThreeQObject.h"

namespace three {
namespace quick {

class Light : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)

protected:
  float _intensity=1;
  three::Light::Ptr _light;

  Light(QObject *parent) : ThreeQObject(parent) {}
  Light(three::Light::Ptr light, QObject *parent)
     : ThreeQObject(light, parent), _light(light) {}

public:
  float intensity() const {return _intensity;}
  void setIntensity(float intensity) {
    if(_intensity != intensity) {
      _intensity = intensity;
      if(_light) _light->intensity() = _intensity;
      emit intensityChanged();
    }
  }

signals:
  void intensityChanged();
};

}
};


#endif //THREEPPQ_LIGHT_H
