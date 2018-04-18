//
// Created by byter on 24.09.17.
//

#ifndef THREEPP_HEMISPHERELIGHT_H
#define THREEPP_HEMISPHERELIGHT_H

#include "Light.h"

namespace three {

class HemisphereLight : public Light
{
  Color _groundColor {1, 1, 1};

protected:
  HemisphereLight(const Color &skyColor, const Color &groundColor, float intensity)
     : Light(skyColor, intensity),
       _groundColor(groundColor)
  {
    updateMatrix();

    _position = math::Vector3(0, 1, 0);
  }

  HemisphereLight(const HemisphereLight &light)
     : Light(light), _groundColor(light.groundColor())
  {
    updateMatrix();
  }

public:
  using Ptr = std::shared_ptr<HemisphereLight>;
  static Ptr make(const Color &skyColor,
                  const Color &groundColor,
                  float intensity = 1)
  {
    return Ptr(new HemisphereLight(skyColor, groundColor, intensity));
  }

  const Color &skyColor() const {return _color;}
  const Color &groundColor() const {return _groundColor;}

  Color &skyColor() {return _color;}
  Color &groundColor() {return _groundColor;}

  HemisphereLight *cloned() const override
  {
    return new HemisphereLight(*this);
  }
};

}
#endif //THREEPP_HEMISPHERELIGHT_H
