//
// Created by byter on 03.09.17.
//

#ifndef THREE_QT_LIGHT_H
#define THREE_QT_LIGHT_H

#include <core/Object3D.h>
#include <helper/Resolver.h>
#include "LightShadow.h"

namespace three {

class Light : public Object3D
{
protected:
  Color _color;
  float _intensity = 1;
  LightShadow::Ptr _shadow;

  Light(light::Resolver::Ptr resolver, const Color &color, float intensity)
     : Object3D(object::ResolverT<Light>::make(*this)),
       lightResolver(resolver),
       _color(color),
       _intensity(intensity)
  {}

public:
  using Ptr = std::shared_ptr<Light>;

  const light::Resolver::Ptr lightResolver;

  const Color &color() const {return _color;}
  float intensity() const {return _intensity;}
  const LightShadow::Ptr shadow() const {return _shadow;};
};

}
#endif //THREE_QT_LIGHT_H
