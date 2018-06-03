//
// Created by byter on 30.09.17.
//

#ifndef THREEPP_SHADERLIB_H
#define THREEPP_SHADERLIB_H

#include <threepp/util/osdecl.h>
#include "ShaderID.h"
#include "UniformsLib.h"

namespace three {
namespace gl {

class UniformValues
{
  enum_map<UniformName, uniformslib::UniformValue::Ptr> values;

  std::unordered_map<std::string, UniformName> _nameRegistry;

public:
  UniformValues() {}

  explicit UniformValues(const uniformslib::LibUniformValues &libUniforms) : values(libUniforms.cloneValues()) {}

  bool contains(UniformName name) const {
    return values.find(name) != values.end();
  }

  bool contains(const std::string &name) const {
    auto found = _nameRegistry.find(name);
    return found != _nameRegistry.end() ? values.find(found->second) != values.end() : false;
  }

  UniformName uniformName(const std::string &name) const {
    auto found = _nameRegistry.find(name);
    if(found != _nameRegistry.end()) {
      auto found2 = values.find(found->second);
      return found2 != values.end() ? found2->first : UniformName::unknown_name;
    }
    return UniformName::unknown_name;
  }

  void needsUpdate(UniformName name, bool value) {
    if(values.count(name) > 0) (*values.at(name)).needsUpdate = value;
  }

  template <typename V>
  UniformValues &set(UniformName name, const V &v) {
    if(values.count(name) > 0)
      *values[name] = v;
    else {
      values[name] = uniformslib::UniformValueT<V>::make(name, v);
    }
    return *this;
  }

  UniformName registered(std::string name) {
    auto found = _nameRegistry.find(name);
    if(found != _nameRegistry.end()) return found->second;
    auto uname = uniformname::registered(_nameRegistry.size());
    _nameRegistry[name] = uname;
    return uname;
  }

  uniformslib::UniformValue &operator[] (UniformName name) {
    return *values.at(name);
  }
};

struct DLX ShaderInfo
{
  const UniformValues uniforms;
  const char * const vertexShader;
  const char * const fragmentShader;

  ShaderInfo(const uniformslib::LibUniformValues &uniforms, const char * vertexShader, const char * fragmentShader);
};

namespace shaderlib {

ShaderInfo DLX get(ShaderID id);

}

}
}
#endif //THREEPP_SHADERLIB_H
