//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_AMBIENTLIGHT_H
#define THREE_QT_AMBIENTLIGHT_H

#include "Light.h"

namespace three {

class AmbientLight : public Light
{
public:
  AmbientLight(const Color &color, float intensity, float distance, float angle)
     : Light(LightResolver<AmbientLight>::make(this), color, intensity, distance, angle)
  {}
};

}
#endif //THREE_QT_AMBIENTLIGHT_H
