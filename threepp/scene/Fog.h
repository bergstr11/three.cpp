//
// Created by byter on 19.08.17.
//

#ifndef THREEPP_FOG_H
#define THREEPP_FOG_H

#include <threepp/Constants.h>
#include <threepp/core/Color.h>

namespace three {

class Fog
{
protected:
  Color _color;

  Fog(const Color &color, fog::Resolver::Ptr resolver) : _color(color), resolver(resolver) {}
  virtual ~Fog() {}

public:
  using Ptr = std::shared_ptr<Fog>;

  fog::Resolver::Ptr resolver;

  const Color &color() const {return _color;}
  Color &color() {return _color;}
};

class DefaultFog : public Fog
{
  float _near;
  float _far;

  DefaultFog(const Color &color, float near, float far)
     : Fog(color, fog::ResolverT<DefaultFog>::make(*this)), _near(near), _far(far) {}

public:
  using Ptr = std::shared_ptr<DefaultFog>;
  static Ptr make(const Color &color, float near, float far) {
    return Ptr(new DefaultFog(color, near, far));
  }

  const float near() const {return _near;}
  float &near() {return _near;}

  const float far() const {return _far;}
  float &far() {return _far;}
};

class FogExp2 : public Fog
{
  float _density;

  FogExp2(const Color &color, float density = 0.00025f)
     : Fog(color, fog::ResolverT<FogExp2>::make(*this)), _density(density) {}

public:
  using Ptr = std::shared_ptr<FogExp2>;
  static Ptr make(const Color &color, float density) {
    return Ptr(new FogExp2(color, density));
  }

  const float density() const {return _density;}
  float &density() {return _density;}
};

}
#endif //THREEPP_FOG_H
