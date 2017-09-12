//
// Created by byter on 19.08.17.
//

#ifndef THREE_QT_FOG_H
#define THREE_QT_FOG_H

#include <core/Color.h>

namespace three {

class Fog
{
  Color _color;
  float _near;
  float _far;

public:
  Fog(const Color &color, float near, float far) : _color(color), _near(near), _far(far) {}
  Fog(const Fog &fog) : _color(fog._color), _near(fog._near), _far(fog._far) {}
  Fog() {}

  const Color &color() const {return _color;}
  Color &color() {return _color;}

  const float near() const {return _near;}
  float &near() {return _near;}

  const float far() const {return _far;}
  float &far() {return _far;}

  bool isNull() const {return _color.isNull();}
};

}
#endif //THREE_QT_FOG_H
