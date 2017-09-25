//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_HEMISPHERELIGHT_H
#define THREE_QT_HEMISPHERELIGHT_H

#include "Light.h"

namespace three {

class HemisphereLight : public Light
{
  Color _groundColor;

public:
  HemisphereLight(const Color &skyColor, const Color &groundColor, float intensity)
     : Light(LightResolver<HemisphereLight>::make(this), skyColor, intensity),
       _groundColor(groundColor), _position(0, 1, 0)
  {
    updateMatrix();

    _groundColor = groundColor;
  }

  const Color &skyColor() {return color();}
  const Color &groundColor() {return _groundColor;}
};

}
#endif //THREE_QT_HEMISPHERELIGHT_H
