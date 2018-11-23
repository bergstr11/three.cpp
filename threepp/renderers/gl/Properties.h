//
// Created by byter on 02.10.17.
//

#ifndef THREEPP_PROPERTIES_H
#define THREEPP_PROPERTIES_H

#include <unordered_map>
#include <string>
#include <type_traits>
#include <threepp/core/Object3D.h>
#include <threepp/scene/Fog.h>
#include <threepp/textures/Texture.h>
#include "Program.h"
#include "shader/ShaderLib.h"

namespace three {
namespace gl {

struct GlProperties
{
  optional<GLuint> image_textureCube;
  bool webglInit = false;
  optional<GLuint> texture;
  optional<float> currentAnisotropy;
  optional<GLuint> version;
};

struct MaterialProperties
{
  Program::Ptr program;
  Fog::Ptr fog;
  std::vector<float> clippingState;
  LightsHash lightsHash;
  size_t numClippingPlanes = 0;
  size_t numIntersection = 0;
  ShaderID shaderID = ShaderID::undefined;
  three::Shader shader;
  std::vector<Uniform::Ptr> uniformsList;
};

class Properties
{
  std::unordered_map<sole::uuid, GlProperties> glProperties;

  std::unordered_map<sole::uuid, MaterialProperties> materialProperties;

public:
  GlProperties &getGlProperties(const sole::uuid &uuid)
  {
    return glProperties[uuid];
  }

  MaterialProperties &getMaterialProperties(const sole::uuid &uuid)
  {
    return materialProperties[uuid];
  }

  template<typename T, typename std::enable_if<!std::is_base_of<Material, T>{}, int>::type = 0>
  GlProperties &get(const T &tee)
  {
    return getGlProperties(tee.uuid);
  }

  template<typename T, typename std::enable_if<!std::is_base_of<Material, T>{}, int>::type = 0>
  GlProperties &get(const std::shared_ptr<T> tee)
  {
    return getGlProperties(tee->uuid);
  }

  template<typename T, typename std::enable_if<!std::is_base_of<Material, T>{}, int>::type = 0>
  void remove(const T &tee)
  {
    glProperties.erase(tee.uuid);
  }

  template<typename T, typename std::enable_if<!std::is_base_of<Material, T>{}, int>::type = 0>
  bool has(const T &tee)
  {
    return glProperties.count(tee.uuid) > 0;
  }

  template<typename T, typename std::enable_if<std::is_base_of<Material, T>{}, int>::type = 0>
  MaterialProperties &get(const T &material)
  {
    return getMaterialProperties(material.uuid);
  }

  template<typename T, typename std::enable_if<std::is_base_of<Material, T>{}, int>::type = 0>
  MaterialProperties &get(const std::shared_ptr<T> material)
  {
    return getMaterialProperties(material->uuid);
  }

  template<typename T, typename std::enable_if<std::is_base_of<Material, T>{}, int>::type = 0>
  void remove(const T &material)
  {
    materialProperties.erase(material.uuid);
  }

  template<typename T, typename std::enable_if<std::is_base_of<Material, T>{}, int>::type = 0>
  bool has(const T &material)
  {
    return materialProperties.count(material.uuid) > 0;
  }

  void clear()
  {
    glProperties.clear();
    materialProperties.clear();
  }
};

}
}
#endif //THREEPP_PROPERTIES_H
