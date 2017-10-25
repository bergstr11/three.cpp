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
#include "../Resolver.h"

namespace three {
namespace gl {
namespace uniforms_cache {

struct EntryBase
{
  virtual void apply() {}
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
  std::unordered_map<unsigned, uniforms_cache::EntryBase::Ptr> lights;

public:

  template <typename L>
  const std::shared_ptr<uniforms_cache::Entry<L>> get(L *light)
  {
    if (lights.find(light->id()) != lights.end()) {

      return std::dynamic_pointer_cast<uniforms_cache::Entry<L>>(lights[ light->id() ]);
    }

    std::shared_ptr<uniforms_cache::Entry<L>> u = std::make_shared<uniforms_cache::Entry<L>>();
    lights[light->id()] = u;

    return u;
  }
};

struct Lights
{
  UniformsCache cache;

  struct State {
    std::vector<Texture::Ptr> directionalShadowMap;
    std::vector<math::Matrix4> directionalShadowMatrix;
    std::vector<uniforms_cache::Entry<DirectionalLight>::Ptr> directional;
    std::vector<Texture::Ptr> spotShadowMap;
    std::vector<math::Matrix4> spotShadowMatrix;
    std::vector<uniforms_cache::Entry<SpotLight>::Ptr> spot;
    std::vector<uniforms_cache::Entry<RectAreaLight>::Ptr> rectArea;
    std::vector<Texture::Ptr> pointShadowMap;
    std::vector<math::Matrix4> pointShadowMatrix;
    std::vector<uniforms_cache::Entry<PointLight>::Ptr> point;
    std::vector<uniforms_cache::Entry<HemisphereLight>::Ptr> hemi;
    Color ambient;
    std::string hash;
  } state;

public:
  void setup(const std::vector<Light::Ptr> &lights, Camera::Ptr camera );
};

}
}
#endif //THREE_QT_LIGHTS_H
