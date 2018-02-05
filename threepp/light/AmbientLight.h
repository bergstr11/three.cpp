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
     : Light(light::ResolverT<AmbientLight>::make(*this), color, intensity)
  {}

public:
  using Ptr = std::shared_ptr<AmbientLight>;
  static Ptr make(const Color &color, float intensity=1.0f) {
    return Ptr(new AmbientLight(color, intensity));
  }
};

}
#endif //THREEPP_AMBIENTLIGHT_H
