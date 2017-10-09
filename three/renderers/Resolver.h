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

#define DEF_RESOLVER() \
struct ResolverBase  { \
  using Ptr = std::shared_ptr<ResolverBase>; \
  virtual void call(Functions &functions) const {} \
  virtual void call(Functions &functions, unsigned index) const {} \
}; \
template <typename L> \
class Resolver {};

#define MK_RESOLVER(Clz, func) \
template <> \
struct Resolver<Clz> : public ResolverBase \
{ \
  Clz *target; \
  Resolver(Clz *target) : target(target) {} \
  static Ptr make(Clz *target) {return Ptr(new Resolver<Clz>(target));} \
  void call(Functions &f) const override {f.func(target);} \
};

namespace three {

class AmbientLight;
class DirectionalLight;
class HemisphereLight;
class PointLight;
class RectAreaLight;
class SpotLight;

template <typename L>
using ResolveFunc = std::function<void(L *)>;

namespace light {

struct Functions
{
  ResolveFunc<AmbientLight> ambient;
  ResolveFunc<DirectionalLight> directional;
  ResolveFunc<HemisphereLight> hemisphere;
  ResolveFunc<PointLight> point;
  ResolveFunc<RectAreaLight> rectarea;
  ResolveFunc<SpotLight> spot;
};

DEF_RESOLVER()

MK_RESOLVER(AmbientLight, ambient)
MK_RESOLVER(DirectionalLight, directional)
MK_RESOLVER(HemisphereLight, hemisphere)
MK_RESOLVER(PointLight, point)
MK_RESOLVER(RectAreaLight, rectarea)
MK_RESOLVER(SpotLight, spot)

}

class CubeTexture;
class Texture;
class Color;

namespace scene {

struct Functions
{
  ResolveFunc<CubeTexture> cubeTexture;
  ResolveFunc<Texture> texture;
  ResolveFunc<Color> color;
};

DEF_RESOLVER()

MK_RESOLVER(CubeTexture, cubeTexture)
MK_RESOLVER(Texture, texture)
MK_RESOLVER(Color, color)

}

}
#endif //THREE_QT_RESOLVER_H
