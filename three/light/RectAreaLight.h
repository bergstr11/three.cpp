//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_RECTAREALIGHT_H
#define THREE_QT_RECTAREALIGHT_H

#include "Light.h"

namespace three {

class RectAreaLight : public Light
{
  float _width;
  float _height;

public:
  RectAreaLight(const Color &color, float intensity, float width, float height, float distance, float angle)
     : Light(light::Resolver<RectAreaLight>::make(this), color, intensity, distance, angle), _width(width), _height(height)
  {
    _position.set( 0, 1, 0 );
    updateMatrix();

    // TODO (abelnation): distance/decay
    // TODO (abelnation): update method for RectAreaLight to update transform to lookat target
    // TODO (abelnation): shadows
  }

  float width() const {return _width;}
  float height() const {return _height;}
};

}
#endif //THREE_QT_RECTAREALIGHT_H
