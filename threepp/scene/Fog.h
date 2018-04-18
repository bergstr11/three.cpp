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

  Fog(const Color &color) : _color(color) {}

public:
  virtual ~Fog() {}

  using Ptr = std::shared_ptr<Fog>;

  const Color &color() const {return _color;}
  Color &color() {return _color;}

  virtual Fog *cloned() const = 0;
};

class DefaultFog : public Fog
{
  float _near;
  float _far;

  DefaultFog(const Color &color, float near, float far)
     : Fog(color), _near(near), _far(far) {}

  DefaultFog(const DefaultFog &fog)
     : Fog(fog._color), _near(fog._near), _far(fog._far) {}

public:
  using Ptr = std::shared_ptr<DefaultFog>;
  static Ptr make(const Color &color, float near, float far) {
    return Ptr(new DefaultFog(color, near, far));
  }

  DefaultFog *cloned() const override {
    return new DefaultFog(*this);
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
     : Fog(color), _density(density) {}

  FogExp2(const FogExp2 &fog)
     : Fog(fog._color), _density(fog._density) {}

public:
  using Ptr = std::shared_ptr<FogExp2>;
  static Ptr make(const Color &color, float density) {
    return Ptr(new FogExp2(color, density));
  }

  FogExp2 *cloned() const override {
    return new FogExp2(*this);
  }

  const float density() const {return _density;}
  float &density() {return _density;}
};

}
#endif //THREEPP_FOG_H
