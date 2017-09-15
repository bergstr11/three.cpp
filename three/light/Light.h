//
// Created by byter on 03.09.17.
//

#ifndef THREE_QT_LIGHT_H
#define THREE_QT_LIGHT_H

#include <core/Object3D.h>
#include "LightShadow.h"

namespace three {

class Light : public Object3D
{
  Color _color;
  float _intensity;

  LightShadow::Ptr _shadow;

protected:
  Light(const Color &color, float intensity) : _color(color), _intensity(intensity) {}

public:
  using Ptr = std::shared_ptr<Light>;
  /*static Ptr make(const Color &color, float intensity) {
    return std::shared_ptr<Light>(new Light(color, intensity));
  }*/

  const Color &color() const {return _color;}
  float intensity() const {return _intensity;}
  const LightShadow::Ptr shadow() const {return _shadow;};

  virtual bool castShadow() = 0;
};

}
#endif //THREE_QT_LIGHT_H
