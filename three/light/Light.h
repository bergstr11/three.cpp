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
  float _intensity;
  float _distance;
  float _angle;
  LightShadow::Ptr _shadow;

  Light(light::Resolver::Ptr resolver, const Color &color, float intensity, float distance, float angle)
     : Object3D(object::ResolverT<Light>::make(*this)),
       lightResolver(resolver),
       _color(color),
       _intensity(intensity),
       _angle(angle),
       _distance(distance)
  {}

public:
  using Ptr = std::shared_ptr<Light>;

  const light::Resolver::Ptr lightResolver;

  const Color &color() const {return _color;}
  float intensity() const {return _intensity;}
  float angle() const {return _angle;}
  float distance() const {return _distance;}
  const LightShadow::Ptr shadow() const {return _shadow;};
};

}
#endif //THREE_QT_LIGHT_H
