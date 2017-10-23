//
// Created by byter on 19.08.17.
//

#ifndef THREE_QT_FOG_H
#define THREE_QT_FOG_H

#include <Constants.h>
#include <core/Color.h>

namespace three {

class Fog
{
protected:
  Color _color;

  Fog(const Color &color, fog::ResolverBase::Ptr resolver) : _color(color), resolver(resolver) {}

public:
  using Ptr = std::shared_ptr<Fog>;

  fog::ResolverBase::Ptr resolver;

  const Color &color() const {return _color;}
  Color &color() {return _color;}
};

class DefaultFog : public Fog
{
  float _near;
  float _far;

  DefaultFog(const Color &color, float near, float far)
     : Fog(color, fog::Resolver<DefaultFog>::make(*this)), _near(near), _far(far) {}

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

public:
  FogExp2(const Color &color, float density)
     : Fog(color, fog::Resolver<FogExp2>::make(*this)), _density(density) {}

  const float density() const {return _density;}
  float &density() {return _density;}
};

}
#endif //THREE_QT_FOG_H
