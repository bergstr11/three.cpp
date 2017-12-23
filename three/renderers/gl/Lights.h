//
// Created by byter on 24.09.17.
//

#ifndef THREE_QT_LIGHTS_H
#define THREE_QT_LIGHTS_H

#include <light/AmbientLight.h>
#include <light/DirectionalLight.h>
#include <light/HemisphereLight.h>
#include <light/PointLight.h>
#include <light/SpotLight.h>
#include <light/RectAreaLight.h>
#include <light/LightShadow.h>
#include <shadow.h>
#include <unordered_map>
#include <helper/Resolver.h>

namespace three {
namespace gl {
namespace lights {

struct EntryBase
{
  virtual void apply() {}
  virtual ~EntryBase() {}
  using Ptr = std::shared_ptr<EntryBase>;
};

template<typename T>
struct Entry {};

template<>
struct Entry<DirectionalLight> : public EntryBase
{
  using Ptr = std::shared_ptr<Entry<DirectionalLight>>;

  math::Vector3 direction;
  Color color;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
};

template<>
struct Entry<SpotLight> : public EntryBase
{
  using Ptr = std::shared_ptr<Entry<SpotLight>>;

  math::Vector3 position;
  math::Vector3 direction;
  Color color;
  float distance = 0;
  float coneCos = 0;
  float penumbraCos = 0;
  float decay = 0;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
};

template<>
struct Entry<PointLight> : public EntryBase
{
  using Ptr = std::shared_ptr<Entry<PointLight>>;

  math::Vector3 position;
  Color color;
  float distance = 0;
  float decay = 0;
  bool shadow = false;
  float shadowBias = 0;
  float shadowRadius = 1;
  math::Vector2 shadowMapSize;
  float shadowCameraNear = 1;
  float shadowCameraFar = 1000;
};

template<>
struct Entry<HemisphereLight> : public EntryBase
{
  using Ptr = std::shared_ptr<Entry<HemisphereLight>>;

  math::Vector3 direction;
  Color skyColor;
  Color groundColor;
};

template<>
struct Entry<RectAreaLight> : public EntryBase
{
  using Ptr = std::shared_ptr<Entry<RectAreaLight>>;

  math::Vector3 position;
  Color color;
  math::Vector3 halfWidth;
  math::Vector3 halfHeight;
};

}

class UniformsCache
{
public:
  std::unordered_map<unsigned, lights::EntryBase::Ptr> lights;

public:

  template <typename L>
  const std::shared_ptr<lights::Entry<L>> get(L *light)
  {
    if (lights.find(light->id()) != lights.end()) {

      return std::dynamic_pointer_cast<lights::Entry<L>>(lights[ light->id() ]);
    }

    std::shared_ptr<lights::Entry<L>> u = std::make_shared<lights::Entry<L>>();
    lights[light->id()] = u;

    return u;
  }
};

using CachedDirectionalLights = std::vector<lights::Entry<DirectionalLight>::Ptr>;
using CachedSpotLights = std::vector<lights::Entry<SpotLight>::Ptr>;
using CachedRectareaLights = std::vector<lights::Entry<RectAreaLight>::Ptr>;
using CachedPointLights = std::vector<lights::Entry<PointLight>::Ptr>;
using CachedHemisphereLights = std::vector<lights::Entry<HemisphereLight>::Ptr>;

struct Lights
{
  UniformsCache cache;

  struct State {
    std::vector<Texture::Ptr> directionalShadowMap;
    std::vector<math::Matrix4> directionalShadowMatrix;
    CachedDirectionalLights directional;
    std::vector<Texture::Ptr> spotShadowMap;
    std::vector<math::Matrix4> spotShadowMatrix;
    CachedSpotLights spot;
    CachedRectareaLights rectArea;
    std::vector<Texture::Ptr> pointShadowMap;
    std::vector<math::Matrix4> pointShadowMatrix;
    CachedPointLights point;
    CachedHemisphereLights hemi;
    Color ambient {0, 0, 0};
    std::string hash;
  } state;

public:
  void setup(const std::vector<Light::Ptr> &lights, Camera::Ptr camera );
};

namespace uniformslib {

//UNIFORM_VALUE_T(CachedDirectionalLights)
//UNIFORM_VALUE_T(CachedSpotLights)
//UNIFORM_VALUE_T(CachedRectareaLights)
//UNIFORM_VALUE_T(CachedPointLights)
//UNIFORM_VALUE_T(CachedHemisphereLights)

}

}
}
#endif //THREE_QT_LIGHTS_H
