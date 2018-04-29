//
// Created by byter on 03.09.17.
//

#ifndef THREEPP_LIGHT_H
#define THREEPP_LIGHT_H

#include <threepp/core/Object3D.h>
#include <threepp/util/Resolver.h>
#include "LightShadow.h"

namespace three {

class Light : public Object3D
{
protected:
  Color _color;
  float _intensity = 1;

  Light(const object::Typer &typer, const Color &color, float intensity)
     : Object3D(), _color(color), _intensity(intensity)
  {
    Object3D::typer = typer;
    Object3D::typer.allow<Light>();
  }

  Light(const Light &light, const object::Typer &typer)
     : Object3D(light), _color(light._color), _intensity(light._intensity)
  {
    Object3D::typer = typer;
    Object3D::typer.allow<Light>();
  }

public:
  using Ptr = std::shared_ptr<Light>;

  Color &color() {return _color;}
  const Color &color() const {return _color;}
  float intensity() const {return _intensity;}
  float &intensity() {return _intensity;}
  virtual const LightShadow::Ptr shadow() const {return nullptr;}
};

}
#endif //THREEPP_LIGHT_H
