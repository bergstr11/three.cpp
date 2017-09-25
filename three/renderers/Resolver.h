//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_RESOLVER_H
#define THREE_QT_RESOLVER_H

#if 0
#include <light/AmbientLight.h>
#include <light/DirectionalLight.h>
#include <light/HemisphereLight.h>
#include <light/PointLight.h>
#include <light/RectAreaLight.h>
#include <light/SpotLight.h>
#endif

#include <memory>
#include <functional>

namespace three {

class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;

class Resolver
{
public:
  using Ptr = std::shared_ptr<Resolver>;
  virtual void resolve() const = 0;
};

struct LightFunctions
{
  template <typename L>
  using Func = std::function<void(L *)>;

  Func<AmbientLight> ambient;
  Func<DirectionalLight> directional;
  Func<HemisphereLight> hemisphere;
  Func<PointLight> point;
  Func<RectAreaLight> rectarea;
  Func<SpotLight> spot;
};

template <typename L>
class LightResolverBase : public Resolver, protected LightFunctions
{
protected:
  L *light;

  explicit LightResolverBase(L *light) : light(light) {}
};

template <typename L>
class LightResolver
{
};

#define MK_RESOLVER(Clz, func) \
template <> \
struct LightResolver<Clz> : public LightResolverBase<Clz> \
{ \
  LightResolver(Clz *light) : LightResolverBase(light) {} \
  static Ptr make(Clz *light) {return Ptr(new LightResolver<Clz>(light));} \
  void resolve() const override {func(light);} \
};

MK_RESOLVER(AmbientLight, ambient)
MK_RESOLVER(DirectionalLight, directional)
MK_RESOLVER(HemisphereLight, hemisphere)
MK_RESOLVER(PointLight, point)
MK_RESOLVER(RectAreaLight, rectarea)
MK_RESOLVER(SpotLight, spot)
}
#endif //THREE_QT_RESOLVER_H
