//
// Created by byter on 19.08.17.
//

#ifndef THREE_QT_FOG_H
#define THREE_QT_FOG_H

#include <Color.h>

namespace three {

class FogExp2
{
  Color _color;
  float _density;

public:
  FogExp2(const Color &color, float density) : _color(color), _density(density) {}
  FogExp2(const FogExp2 &fog) : _color(fog._color), _density(fog._density) {}

  const Color &color() const {return _color;}
  Color &color() {return _color;}

  const float density() const {return _density;}
  float &density() {return _density;}
};

}
#endif //THREE_QT_FOG_H
