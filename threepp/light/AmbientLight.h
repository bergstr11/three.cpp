//
// Created by byter on 24.09.17.
//

#ifndef THREEPP_AMBIENTLIGHT_H
#define THREEPP_AMBIENTLIGHT_H

#include "Light.h"

namespace three {

class AmbientLight : public Light
{
protected:
  AmbientLight(const Color &color, float intensity)
     : Light(object::Typer(this), color, intensity)
  {}

  AmbientLight(const AmbientLight &light) : Light(light, object::Typer(this))
  {}

public:
  using Ptr = std::shared_ptr<AmbientLight>;
  static Ptr make(const Color &color, float intensity=1.0f) {
    return Ptr(new AmbientLight(color, intensity));
  }

  AmbientLight *cloned() const override
  {
    return new AmbientLight(*this);
  }
};

}
#endif //THREEPP_AMBIENTLIGHT_H
