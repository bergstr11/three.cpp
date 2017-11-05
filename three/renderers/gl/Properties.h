//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_PROPERTIES_H
#define THREE_QT_PROPERTIES_H

#include <unordered_map>
#include <string>
#include <core/Object3D.h>
#include <textures/Texture.h>
#include "Program.h"
#include "shader/ShaderLib.h"

namespace three {
namespace gl {

class Property
{
  friend class Properties;
  friend class Textures;
  friend class Renderer_impl;

  enum Type {glu, glup, s, f, u} type;
  union {
    GLuint gluint_value;
    GLuint *gluintp_value;
    std::string string_value;
    float float_value;
  };

  void release()
  {
    switch (type) {
      case glup:
        delete[] gluintp_value;
        type = u;
        break;
      default:
        break;
    }
  }

public:
  explicit Property() : gluint_value(0L), type(u) {}
  explicit Property(GLuint v) : gluint_value(v), type(glu) {}
  explicit Property(GLuint *v) : gluintp_value(v), type(glup) {}
  explicit Property(std::string v) : string_value(v), type(s) {}
  explicit Property(float v) : float_value(v), type(f) {}

  Property(const Property &prop) : type(prop.type) {
    type = prop.type;
    switch(prop.type) {
      case glu:
        gluint_value = prop.gluint_value;
        break;
      case glup:
        gluintp_value = prop.gluintp_value;
        break;
      case s:
        new (&string_value) std::string();
        string_value = prop.string_value;
        break;
      case f:
        float_value = prop.float_value;
        break;
      default:
        break;
    }
  }

  ~Property() {
    switch(type) {
      case glup:
        delete [] gluintp_value;
        break;
      case s:
        string_value.~basic_string();
        break;
      default:
        break;
    }
  }

  Property &operator =(GLuint v) {
    release();
    gluint_value = v; type = glu;
  }
  Property &operator =(GLuint *v) {
    release();
    gluintp_value = v; type = glup;
  }
  Property &operator =(std::string v) {
    release();
    string_value = v; type = s;
  }
  Property &operator =(float v) {
    release();
    float_value = v; type = f;
  }

  explicit operator GLuint *() const
  {
    switch(type) {
      case glup:
        return gluintp_value;
      default:
        throw std::logic_error("pointer cannot be cast to ");
    }
  }
  explicit operator GLuint() const
  {
    switch(type) {
      case glu:
        return gluint_value;
      case s:
        return (GLuint)std::stoul(string_value);
      case f:
        return (GLuint)float_value;
    }
  }
  explicit operator std::string() const
  {
    switch(type) {
      case glu:
        return std::to_string(gluint_value);
      case s:
        return string_value;
      case f:
        return std::to_string(float_value);
    }
  }
  explicit operator float() const
  {
    switch(type) {
      case glu:
        return gluint_value;
      case s:
        return std::stof(string_value);
      case f:
        return float_value;
    }
  }
};

enum PropertyKey
{
  __image__webglTextureCube, __webglInit, __webglTexture, __webglFramebuffer, __webglDepthbuffer,
  __version
};

struct MaterialProperties
{
  Program::Ptr program;
  Fog::Ptr fog;
  std::vector<float> clippingState;
  std::string lightsHash;
  size_t numClippingPlanes;
  size_t numIntersection;
  three::Shader *shader;
  std::vector<Uniform::Ptr> uniformsList;
};

class Properties
{
  std::unordered_map<sole::uuid, std::unordered_map<PropertyKey, Property>> properties = {};

  std::unordered_map<sole::uuid, MaterialProperties> materialProperties = {};

public:
  std::unordered_map<PropertyKey, Property> &get(const sole::uuid &uuid)
  {
    if(properties.find(uuid) == properties.end())
      properties.emplace(uuid, std::unordered_map<PropertyKey, Property>());

    return properties[uuid];
  }

  bool has(const sole::uuid &uuid, PropertyKey key)
  {
    if(properties.find(uuid) != properties.end()) {
      auto &props = properties[uuid];
      return props.find(key) != props.end();
    }
    return false;
  }

  MaterialProperties &getMaterial(const Material::Ptr &material)
  {
    return materialProperties[material->uuid];
  }

  template <typename T>
  std::unordered_map<PropertyKey, Property> &get(const T *object)
  {
    return get(object->uuid);
  }

  template <typename T>
  std::unordered_map<PropertyKey, Property> &get(const std::shared_ptr<T> object)
  {
    return get(object->uuid);
  }

  template <typename T>
  bool has(const T *object, PropertyKey key) {
    return has(object->uuid, key);
  }

  template <typename T>
  bool has(const std::shared_ptr<T> object, PropertyKey key) {
    return has(object->uuid, key);
  }

  template <typename T>
  void remove(const T *object)
  {
    properties.erase(object->uuid);
  }

  void clear()
  {
    properties.clear();
  }
};

}
}
#endif //THREE_QT_PROPERTIES_H
