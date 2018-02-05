//
// Created by byter on 24.09.17.
//

#ifndef THREEPP_RECTAREALIGHT_H
#define THREEPP_RECTAREALIGHT_H

#include "Light.h"

namespace three {

class RectAreaLight : public Light
{
  float _width;
  float _height;

public:
  RectAreaLight(const Color &color, float intensity, float width, float height)
     : Light(light::ResolverT<RectAreaLight>::make(*this), color, intensity), _width(width), _height(height)
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
#endif //THREEPP_RECTAREALIGHT_H
