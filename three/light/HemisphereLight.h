//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_HEMISPHERELIGHT_H
#define THREE_QT_HEMISPHERELIGHT_H

#include "Light.h"

namespace three {

class HemisphereLight : public Light
{
public:
  HemisphereLight(const Color &color, float intensity, float distance, float angle)
     : Light(color, intensity, distance, angle)
  {}
};

}
#endif //THREE_QT_HEMISPHERELIGHT_H
